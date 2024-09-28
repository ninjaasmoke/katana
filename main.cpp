#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QMessageBox>
#include <curl/curl.h>
#include <string>

// Function to handle incoming data from libcurl
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to fetch the URL content using libcurl
std::string fetchURL(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            readBuffer = "Error: " + std::string(curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
    return readBuffer;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Create main window
    QWidget window;
    window.setWindowTitle("Simple C++ Browser");

    // Layout
    QVBoxLayout* layout = new QVBoxLayout(&window);

    // URL input field
    QLineEdit* urlInput = new QLineEdit();
    urlInput->setPlaceholderText("Enter URL (e.g., https://www.example.com)");
    layout->addWidget(urlInput);

    // Fetch button
    QPushButton* fetchButton = new QPushButton("Fetch URL");
    layout->addWidget(fetchButton);

    // Text area to display result
    QTextEdit* resultArea = new QTextEdit();
    resultArea->setReadOnly(true);
    layout->addWidget(resultArea);

    // Connect the fetch button to a function
    QObject::connect(fetchButton, &QPushButton::clicked, [&]() {
        QString url = urlInput->text();
        if (url.isEmpty()) {
            QMessageBox::warning(&window, "Error", "Please enter a valid URL.");
            return;
        }

        // Fetch the URL content
        std::string content = fetchURL(url.toStdString());

        // Display the result in the text area
        resultArea->setText(QString::fromStdString(content));
    });

    // Show the window
    window.show();

    return app.exec();
}
