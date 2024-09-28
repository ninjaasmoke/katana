#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QWebEngineView>
#include <QMessageBox>
#include <QProgressBar>
#include <QTimer>
#include <QUrl>
#include <curl/curl.h>
#include <string>
#include <thread>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QWebEngineSettings>

struct FetchResult
{
    std::string content;
    std::string finalUrl;
};

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

FetchResult fetchURL(const std::string &url)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    FetchResult result;

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            char *escapedQuery = curl_easy_escape(curl, url.c_str(), url.length());
            result = fetchURL("https://www.google.com/search?q=" + std::string(escapedQuery));
            curl_free(escapedQuery);
        }
        else
        {
            result.content = readBuffer;

            qDebug() << "Fetched content size:" << result.content.size();

            char *final_url;
            curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &final_url);
            result.finalUrl = final_url ? final_url : url;
        }
        curl_easy_cleanup(curl);
    }
    return result;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Katana Browser");
    window.resize(900, 700);
    QWebEngineView *webView = new QWebEngineView();

    QWebEngineSettings *settings = webView->settings();
    settings->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);
    settings->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, false);
    settings->setAttribute(QWebEngineSettings::WebGLEnabled, true);

    QVBoxLayout *layout = new QVBoxLayout(&window);
    QHBoxLayout *hLayout = new QHBoxLayout();

    QPushButton *backButton = new QPushButton("<");
    backButton->setStyleSheet("font-size: 14px; background-color: transparent; color: black; margin-right: 18px; margin-left: 10px;");
    backButton->setCursor(Qt::PointingHandCursor);
    hLayout->addWidget(backButton);

    QPushButton *forwardButton = new QPushButton(">");
    forwardButton->setStyleSheet("font-size: 14px; background-color: transparent; color: black; margin-right: 10px;");
    forwardButton->setCursor(Qt::PointingHandCursor);
    hLayout->addWidget(forwardButton);

    QLineEdit *urlInput = new QLineEdit();
    urlInput->setPlaceholderText("Search or type a URL");
    urlInput->setStyleSheet("padding: 10px; font-size: 14px; border-radius: 5px;");
    hLayout->addWidget(urlInput);

    QPushButton *fetchButton = new QPushButton("Go");
    fetchButton->setStyleSheet("font-size: 14px; padding: 10px; background-color: #333; color: white; border-radius: 5px;");
    fetchButton->setCursor(Qt::PointingHandCursor);
    hLayout->addWidget(fetchButton);

    QObject::connect(urlInput, &QLineEdit::returnPressed, fetchButton, &QPushButton::click);

    hLayout->setSpacing(10);
    hLayout->setContentsMargins(15, 0, 15, 0);

    layout->addLayout(hLayout);

    webView->setStyleSheet("background-color: palette(window);");
    layout->addWidget(webView);

    layout->setContentsMargins(0, 15, 0, 15);
    layout->setSpacing(10);

    QObject::connect(fetchButton, &QPushButton::clicked, [&]()
                     {
    QString url = urlInput->text();
    if (url.isEmpty()) {
        QMessageBox::warning(&window, "Error", "Please enter a valid URL.");
        return;
    }

    fetchButton->setEnabled(false);
    urlInput->setEnabled(false);

    QtConcurrent::run([url, &webView, &urlInput, &fetchButton]() {
        FetchResult result = fetchURL(url.toStdString());

        QMetaObject::invokeMethod(qApp, [&, result]() {
            QUrl finalUrl(QString::fromStdString(result.finalUrl));
            webView->setHtml(QString::fromStdString(result.content), finalUrl);

            urlInput->setText(QString::fromStdString(result.finalUrl));
            fetchButton->setEnabled(true);
            urlInput->setEnabled(true);
        });
    }); });

    QObject::connect(backButton, &QPushButton::clicked, webView, &QWebEngineView::back);
    QObject::connect(forwardButton, &QPushButton::clicked, webView, &QWebEngineView::forward);

    window.show();

    return app.exec();
}