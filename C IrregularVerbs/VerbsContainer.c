//
// Created by mfran on 16/01/2020.
//

#include <stdio.h>
#include "VerbsContainer.h"
#include "SQLite/sqlite3.h"

static sqlite3* db = NULL;
static sqlite3_stmt* sql_insert_verb = NULL;
static sqlite3_stmt* sql_count = NULL;
static sqlite3_stmt* sql_list_by_first_letter = NULL;
static sqlite3_stmt* sql_search = NULL;

/**
 * Runs a simple statement that shall not fail and returns true.
 *
 * @param prepared The statement with all parameters ready.
 * @return True if success of query, False otherwise.
 */
static bool runSimpleQuery(sqlite3_stmt* prepared) {
    return
        (sqlite3_step(prepared) == SQLITE_DONE) &&
        (sqlite3_reset(prepared) == SQLITE_OK);
}

/**
 * Runs a statement that shall not fail.
 * Stores the results (in order) in the QueryResults.
 *
 * @param stmt The statement which shall be ready.
 * @param output The object on which results will be written.
 */
static void makeQueryResults(sqlite3_stmt* stmt, QueryResults* output) {

}

static void bindToStmt(sqlite3_stmt* stmt, int id, MultiStrings* multis) {
    STRING str;
    makeStringFromMultiStrings(multis, &str);
    sqlite3_bind_text(stmt, id, str, -1, NULL);
    freeStringFromMultiStrings(str);
}

bool container_addVerb(const Verb* verb) {
    bindToStmt(sql_insert_verb, 1, verb->infinitive);
    bindToStmt(sql_insert_verb, 2, verb->translation);
    bindToStmt(sql_insert_verb, 3, verb->time1);
    bindToStmt(sql_insert_verb, 4, verb->time2);
    return runSimpleQuery(sql_insert_verb);
}

bool container_addVerbs(const Verb** verbs, size_t number) {
    // 1 - begin exclusive transaction
    // 2 - add verbs within this transaction
    // 3 - commit

    if (sqlite3_exec(db, "BEGIN EXCLUSIVE TRANSACTION;", NULL, NULL, NULL) == SQLITE_OK) {
        bool results = true;
        for (size_t i = 0; i < number; i++) {
            if (!container_addVerb(verbs[i])) {
                results = false;
            }
        }
        sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
        return results;
    } else return false;
}

QueryResults* container_getVerbsByInfinitive(STRING);
QueryResults* container_getVerbsByTranslation(STRING);
QueryResults* container_getVerbsByTime1(STRING);
QueryResults* container_getVerbsByTime2(STRING);

void container_freeQueryResults(QueryResults* qr) {
    for (size_t i = 0; i < qr->length; i++) {
        freeVerb(qr->results[i]);
    }
    free(qr->results);
    qr->results = NULL;
    qr->length = 0;
}

void container_start_up() {
    if (sqlite3_open_v2(":memory:", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL) == SQLITE_OK) {
        sqlite3_stmt* sql_create_table;
        STRING statement_text = "CREATE TABLE Verbs (infinitive TEXT PRIMARY KEY, translation TEXT, time1 TEXT, time2 TEXT) ;";
        if (
                (sqlite3_prepare_v2(db, statement_text, -1, &sql_create_table, NULL) == SQLITE_OK) &&
                runSimpleQuery(sql_create_table))
        {
            sqlite3_finalize(sql_create_table);

            // Prepare all statements
            if (
                    (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM Verbs ;", -1, &sql_count, NULL) == SQLITE_OK) &&
                    (sqlite3_prepare_v2(db, "INSERT INTO Verbs VALUES ( ?1, ?2, ?3, ?4 ) ;", -1, &sql_insert_verb, NULL) == SQLITE_OK) &&
                    (sqlite3_prepare_v2(db, "SELECT * FROM Verbs ORDER BY infinitive ;", -1, &sql_list_by_first_letter, NULL) == SQLITE_OK) &&
                    (sqlite3_prepare_v2(db, "SELECT * FROM Verbs WHERE (instr(infinitive, $s) > 0) OR (instr(translation, $s) > 0) OR (instr(time1, $s) > 0) OR (instr(time2, $s) > 0) ORDER BY infinitive ;", -1, &sql_search, NULL) == SQLITE_OK))
            {
            } else {
                printf("\n%s\n", sqlite3_errmsg(db));
            }
        } else {
            printf("CREATE failed!");
        }

    } else {
        printf("OPEN Failed");
        printf("\n%s\n", sqlite3_errmsg(db));
    }
}

void container_shut_down() {
    sqlite3_finalize(sql_list_by_first_letter);
    sqlite3_finalize(sql_insert_verb);
    sqlite3_finalize(sql_count);
    sqlite3_finalize(sql_search);
    sqlite3_close(db);
}
