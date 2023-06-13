#pragma once

#include <unordered_map>

#include "json.h"
#include "database.h"

namespace abcserver
{

using sql_storage = std::unordered_map<std::string,
    abcserver::json::value(*)(abcserver::db::database&, const abcserver::json::object&)>;

}
