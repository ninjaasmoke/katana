#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QMessageBox>
#include <curl/curl.h>
#include <string>

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

std::string fetchURL(const std::string &url)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            readBuffer = "Error: " + std::string(curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
    return readBuffer;
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

    QTextEdit *resultArea = new QTextEdit();
    resultArea->setReadOnly(true);

    resultArea->setStyleSheet("font-size: 14px;");
    layout->addWidget(resultArea);

    layout->setContentsMargins(0, 15, 0, 15);
    layout->setSpacing(10);

    QObject::connect(fetchButton,
                     &QPushButton::clicked, [&]()
                     {
        QString url = urlInput->text();
        if (url.isEmpty()) {
            QMessageBox::warning(&window, "Error", "Please enter a valid URL.");
            return;
        }
        std::string content = fetchURL(url.toStdString());
        resultArea->setText(QString::fromStdString(content)); });

    window.show();

    return app.exec();
}
