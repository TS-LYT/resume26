#include <iostream>
#include <sqlite3.h>
#include <string>

// 本示例演示 SQLite3 的最常用流程：
// 1. 打开数据库文件。
// 2. 创建表。
// 3. 使用预编译语句插入数据。
// 4. 查询数据并逐行读取。
// 5. 关闭数据库。
//
// SQLite3 是 C 语言库，所以很多接口都会返回 int 错误码。
// C++ 里调用 SQLite3 时，最重要的是：每一步都检查返回值。

static bool exec_sql(sqlite3* db, const std::string& sql)
{
    char* err_msg = nullptr;

    // sqlite3_exec 适合执行“不需要逐行读取结果”的 SQL，例如建表、删除表。
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        std::cerr << "sqlite3_exec failed: " << err_msg << "\n";
        sqlite3_free(err_msg);
        return false;
    }

    return true;
}

static bool insert_sample(sqlite3* db, int sensor_id, int value, const std::string& time_text)
{
    const char* sql =
        "INSERT INTO samples(sensor_id, value, time_text) VALUES(?, ?, ?);";

    sqlite3_stmt* stmt = nullptr;

    // prepare 可以把 SQL 编译成语句对象。
    // ? 是占位符，后面用 bind 绑定实际参数，避免手写字符串拼接。
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "prepare insert failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    // SQLite 占位符编号从 1 开始，不是从 0 开始。
    sqlite3_bind_int(stmt, 1, sensor_id);
    sqlite3_bind_int(stmt, 2, value);
    sqlite3_bind_text(stmt, 3, time_text.c_str(), -1, SQLITE_TRANSIENT);

    // INSERT/UPDATE/DELETE 执行成功时，sqlite3_step 返回 SQLITE_DONE。
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "insert failed: " << sqlite3_errmsg(db) << "\n";
        sqlite3_finalize(stmt);
        return false;
    }

    // finalize 释放语句对象。prepare 成功后，最后都要 finalize。
    sqlite3_finalize(stmt);
    return true;
}

static bool query_samples(sqlite3* db)
{
    const char* sql =
        "SELECT id, sensor_id, value, time_text FROM samples ORDER BY id;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "prepare query failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    std::cout << "id sensor value time\n";

    // SELECT 查询每读到一行，sqlite3_step 返回 SQLITE_ROW。
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int sensor_id = sqlite3_column_int(stmt, 1);
        int value = sqlite3_column_int(stmt, 2);
        const unsigned char* time_text = sqlite3_column_text(stmt, 3);

        std::cout << id << " "
                  << sensor_id << " "
                  << value << " "
                  << reinterpret_cast<const char*>(time_text) << "\n";
    }

    if (rc != SQLITE_DONE) {
        std::cerr << "query failed: " << sqlite3_errmsg(db) << "\n";
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

int main()
{
    sqlite3* db = nullptr;

    // 数据库文件不存在时，sqlite3_open 会自动创建。
    int rc = sqlite3_open("sensor.db", &db);
    if (rc != SQLITE_OK) {
        std::cerr << "open database failed: " << sqlite3_errmsg(db) << "\n";
        sqlite3_close(db);
        return 1;
    }

    const std::string create_table_sql =
        "CREATE TABLE IF NOT EXISTS samples("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "sensor_id INTEGER NOT NULL,"
        "value INTEGER NOT NULL,"
        "time_text TEXT NOT NULL"
        ");";

    if (!exec_sql(db, create_table_sql)) {
        sqlite3_close(db);
        return 1;
    }

    // 为了让每次运行结果容易观察，先清空旧数据。
    if (!exec_sql(db, "DELETE FROM samples;")) {
        sqlite3_close(db);
        return 1;
    }

    insert_sample(db, 1, 31, "2026-05-26 10:00:00");
    insert_sample(db, 1, 33, "2026-05-26 10:00:01");
    insert_sample(db, 2, 58, "2026-05-26 10:00:02");

    query_samples(db);

    sqlite3_close(db);
    return 0;
}
