#include "filesession.hpp"
#include "ftpclient.h"
#include "stsyncqueue.hpp"
#include <fstream>

void *work(void *args) {
    auto *fileList = reinterpret_cast<STSyncQueue<std::string> *>(args);
    try {
        FTPClient cli("124.207.27.34", "21", "gzftpqas01", "001testgz");
        cli.open();
        cli.login();
        cli.change_dir("/OUT/stockout/");
        std::string fn;
        while (!fileList->empty()) {
            fileList->take(fn);
            try{
                std::istream &is(cli.begin_download(fn));
                FileSession newFile(fn, O_CREAT | O_WRONLY | O_TRUNC);
                newFile.io() << is.rdbuf();
                cli.end_download();
            }
            catch(const std::exception& e)
            {
                fileList->put(fn);
                throw e;
            }
        }
        cli.logout();
    } catch (...) {
        perror("error");
    }
    return nullptr;
}

void *get_list(void *args) {
    FTPClient cli("124.207.27.34", "21", "gzftpqas01", "001testgz");
    cli.open();
    cli.login();
    cli.change_dir("/OUT/stockout/");
    std::ofstream f("list.txt", std::ios::binary);
    std::istream &is(cli.begin_list("."));
    std::string fn;
    while (getline(is, fn)) {
        if (fn.find(".xml") != fn.npos) {
            fn.erase(fn.find('\r'), fn.npos);
            f << fn << '\n';
        }
    }
    cli.end_list();
    f.flush();
    cli.logout();
    return 0;
}

int main() {
    st_set_eventsys(ST_EVENTSYS_ALT);
    st_init();
    std::ifstream f("list.txt", std::ios::binary);
    std::string fn;
    STSyncQueue<std::string> fileList(50000);
    
    while (std::getline(f, fn)) {
        fileList.put(fn);
    }
    f.close();
    
    std::vector<st_thread_t> tds;
    for (int i = 0; i < 50; ++i)
        tds.push_back(st_thread_create(work, &fileList, 1, 0));
    //        tds.push_back(st_thread_create(get_list, nullptr, 1, 0));
    for (auto td : tds)
        st_thread_join(td, nullptr);
    return 0;
}
