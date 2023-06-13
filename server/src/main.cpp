#include <auth_service.h>
#include <config_service.h>
#include <database_service.h>
#include <log_service.h>
#include <method_service.h>
#include <web_service.h>

#include <dynamic_data.h>

int main(int argc, char *argv[])
{
    using namespace abcserver;

    LogService log_service;

    if(argc < 2)
    {
        log_service.Error("%1% [config path]", argv[0]);
        return 0;
    }

    try
    {
        boost::asio::io_service io_service;

        ConfigService config_service{argv[1]};

        DatabaseService database_service{io_service};

        database_service.AppendDatabase(config_service.Get<std::string>("database.host"),
                                        config_service.Get<std::string>("database.port"),
                                        config_service.Get<std::string>("database.name"),
                                        config_service.Get<std::string>("database.user"),
                                        config_service.Get<std::string>("database.pass"),
                                        config_service.Get<int>("database.count"));

        AuthService auth_service{io_service};

        auth_service.SetDatabaseService(&database_service);

        auth_service.Run();

        MethodService method_service;

        method_service.LoadMethods();

        WebService web_service{io_service,
                              static_cast<unsigned short>(config_service.Get<int>("webservice.port")),
                              static_cast<unsigned short>(config_service.Get<int>("webservice.wsport"))};

        web_service.SetAuthService(&auth_service);
        web_service.SetConfigService(&config_service);
        web_service.SetDatabaseService(&database_service);
        web_service.SetLogService(&log_service);
        web_service.SetMethodService(&method_service);

        web_service.SetRequestHandler(dynamic_data::process);

        auto thread_count = config_service.Get<int>("webservice.threads", 3);

        std::vector<std::thread> threads;

        for(std::size_t i = 0; i < thread_count; ++i)
        {
            threads.push_back(std::thread{&WebService::Run, &web_service});
        }

        for(auto& thread : threads)
        {
            thread.join();
        }
    }
    catch(const std::exception& ex)
    {
        log_service.Error("%1%", ex.what());
    }

    return 0;
}
