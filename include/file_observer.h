#pragma once
#include "observer.h"
#include <fstream>
#include <string>

class FileObserver : public Observer {
    public:
        FileObserver(const std::string& filename) : filename(filename) {}
        
        void notify(const std::string& event) override {
            std::ofstream file(filename, std::ios::app);
            if (file.is_open()) {
                file << event << std::endl;
            }
        }
private:
    std::string filename;
};