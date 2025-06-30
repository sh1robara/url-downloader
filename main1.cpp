#include <iostream>
#include <stdexcept>
#include <memory>

int main(int argc, char* argv[]) {
    try {
        // 1. Проверка аргументов командной строки
        if (argc != 4) {
            std::cerr << "Usage: " << argv[0] 
                      << " <url_file> <output_dir> <thread_count>\n";
            return 1;
        }

        const std::string urlFile = argv[1];
        const std::string outputDir = argv[2];
        const int threadCount = std::stoi(argv[3]);

        // 2. Инициализация логгера
        Logger logger;
        logger.log("Program started");
        logger.log("Parameters:");
        logger.log("  URL file: " + urlFile);
        logger.log("  Output dir: " + outputDir);
        logger.log("  Thread count: " + std::to_string(threadCount));

        // 3. Проверка параметров
        if (threadCount < 1 || threadCount > 999) {
            throw std::runtime_error("Thread count must be between 1 and 999");
        }

        // 4. Создание директории для загрузок
        if (!Utils::createDirectoryIfNotExists(outputDir)) {
            throw std::runtime_error("Failed to create output directory: " + outputDir);
        }

        // 5. Чтение URL из файла
        auto urls = Utils::readUrlsFromFile(urlFile);
        if (urls.empty()) {
            logger.log("No URLs found in file");
            return 0;
        }
        logger.log("Found " + std::to_string(urls.size()) + " URLs to download");

        // 6. Инициализация загрузчика и пула потоков
        ThreadPool pool(threadCount);
        FileDownloader downloader(outputDir, logger);

        // 7. Запуск загрузок
        for (const auto& url : urls) {
            pool.enqueue([url, &downloader]() {
                downloader.downloadFile(url);
            });
        }

        // 8. Ожидание завершения
        pool.waitAll();
        logger.log("All downloads completed");
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}