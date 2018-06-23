#pragma once

#include "http.hpp"

#include <sqlite3.h>

#include <array>
#include <memory>
#include <string>
#include <vector>

#include <cstdint>

typedef enum {
    PresenceUnknown,
    PresenceIncomplete,
    PresenceInstalling,
    PresenceInstalled,
    PresenceMissing,
    PresenceGamePresent,
} DbPresence;

typedef enum {
    SortByTitle,
    SortByRegion,
    SortByName,
    SortBySize,
} DbSort;

typedef enum {
    SortAscending,
    SortDescending,
} DbSortOrder;

typedef enum {
    DbFilterRegionASA = 0x01,
    DbFilterRegionEUR = 0x02,
    DbFilterRegionJPN = 0x04,
    DbFilterRegionUSA = 0x08,

    // TODO: implement these two
    DbFilterInstalled = 0x10,
    DbFilterMissing = 0x20,

    DbFilterAllRegions = DbFilterRegionUSA | DbFilterRegionEUR |
                         DbFilterRegionJPN | DbFilterRegionASA,
    DbFilterAll = DbFilterAllRegions | DbFilterInstalled | DbFilterMissing,
} DbFilter;

typedef struct
{
    DbPresence presence;
    std::string titleid;
    std::string content;
    uint32_t flags;
    std::string name;
    std::string name_org;
    std::string zrif;
    std::string url;
    bool has_digest;
    std::array<uint8_t, 32> digest;
    int64_t size;
} DbItem;

typedef enum {
    RegionASA,
    RegionEUR,
    RegionJPN,
    RegionUSA,
    RegionUnknown,
} GameRegion;

typedef enum {
    ModeGames,
    ModeUpdates,
    ModeDlcs,
    ModePsxGames,
    ModePspGames,
} Mode;

typedef struct Config Config;

struct SqliteClose
{
    void operator()(sqlite3* s)
    {
        sqlite3_close(s);
    }
};

class TitleDatabase
{
public:
    TitleDatabase(Mode mode, const std::string& dbPath);

    void reload(uint32_t region_filter, const std::string& search);

    void update(Http* http, const char* update_url);
    void get_update_status(uint32_t* updated, uint32_t* total);

    void configure(const char* search, const Config* config);

    uint32_t count();
    uint32_t total();
    DbItem* get(uint32_t index);
    DbItem* get_by_content(const char* content);

private:
    static constexpr auto MAX_DB_SIZE = 4 * 1024 * 1024;
    static constexpr auto MAX_DB_ITEMS = 8192;

    Mode mode;
    uint32_t db_total;
    uint32_t db_size;
    uint32_t _title_count;

    std::vector<DbItem> db;

    std::unique_ptr<sqlite3, SqliteClose> _sqliteDb = nullptr;

    void parse_tsv_file(std::string& db_data);
};

GameRegion pkgi_get_region(const std::string& titleid);
