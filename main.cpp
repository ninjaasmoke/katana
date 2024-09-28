#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QMessageBox>
#include <QProgressBar>
#include <QTimer>
#include <curl/curl.h>
#include <string>
#include <thread>

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
            result.content = "Error: " + std::string(curl_easy_strerror(res));
            result.finalUrl = url;
        }
        else
        {
            result.content = readBuffer;

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

    QVBoxLayout *layout = new QVBoxLayout(&window);

    QHBoxLayout *hLayout = new QHBoxLayout();

    QLineEdit *urlInput = new QLineEdit();
    urlInput->setPlaceholderText("type a URL here");
    urlInput->setStyleSheet("padding: 10px; font-size: 14px; border-radius: 5px;");
    hLayout->addWidget(urlInput);

    QPushButton *fetchButton = new QPushButton("Go");
    fetchButton->setStyleSheet("font-size: 14px; padding: 10px; background-color: grey; color: white; border-radius: 5px;");
    hLayout->addWidget(fetchButton);

    QObject::connect(urlInput, &QLineEdit::returnPressed, fetchButton, &QPushButton::click);

    hLayout->setSpacing(10);
    hLayout->setContentsMargins(15, 0, 15, 0);

    layout->addLayout(hLayout);

    QWidget *loadingBarBackground = new QWidget();
    loadingBarBackground->setStyleSheet(
        "background-color: red;"
        "border-radius: 2px;"
        "height: 2px;");
    layout->addWidget(loadingBarBackground);

    QWidget *loadingBarProgress = new QWidget(loadingBarBackground);
    loadingBarProgress->setStyleSheet(
        "background-color: #05B8CC;"
        "border-radius: 2px;"
        "height: 2px;");
    loadingBarProgress->setFixedWidth(50);


    QTextEdit *resultArea = new QTextEdit();
    resultArea->setReadOnly(true);
    resultArea->setStyleSheet("font-size: 14px;");
    layout->addWidget(resultArea);

    layout->setContentsMargins(0, 15, 0, 15);
    layout->setSpacing(10);

    QObject::connect(fetchButton, &QPushButton::clicked, [&]()
                     {
        QString url = urlInput->text();
        if (url.isEmpty()) {
            QMessageBox::warning(&window, "Error", "Please enter a valid URL.");
            return;
        }

        loadingBarBackground->show();
        fetchButton->setEnabled(false);
        urlInput->setEnabled(false);

        std::thread([url, &resultArea, &urlInput, &loadingBarBackground, &fetchButton]() {
            FetchResult result = fetchURL(url.toStdString());
            
            QMetaObject::invokeMethod(qApp, [&, result]() {
                resultArea->setText(QString::fromStdString(result.content));
                urlInput->setText(QString::fromStdString(result.finalUrl));
                loadingBarBackground->hide();
                fetchButton->setEnabled(true);
                urlInput->setEnabled(true);
            });
        }).detach(); });

    window.show();

    return app.exec();
}