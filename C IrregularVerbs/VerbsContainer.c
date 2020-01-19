//
// Created by mfran on 16/01/2020.
//

#include <stdint.h>
#include <stdio.h>
#include "VerbsContainer.h"
#include "SQLite/sqlite3.h"

/* The SQLite connexion. */
static sqlite3* db = NULL;

/* The SQL queries that may be run once or many times. */
static sqlite3_stmt* sql_insert_verb = NULL;
static STRING sql_insert_verb_code = "INSERT INTO Verbs VALUES ( ?1, ?2, ?3, ?4 ) ;";
static sqlite3_stmt* sql_count = NULL;
static STRING sql_count_code = "SELECT COUNT(*) FROM Verbs ;";
static sqlite3_stmt* sql_list_by_first_letter = NULL;
static STRING sql_list_by_first_letter_code = "SELECT * FROM Verbs ORDER BY infinitive ;";
static sqlite3_stmt* sql_search = NULL;
static STRING sql_search_code = "SELECT * FROM Verbs WHERE (instr(infinitive, $s) > 0) OR (instr(translation, $s) > 0) OR (instr(time1, $s) > 0) OR (instr(time2, $s) > 0) ORDER BY infinitive ;";

/* When verbs are returned by queries using "select *", here are the indices of columns. */
static const int INDEX_INFINITIVE = 0;
static const int INDEX_TRANSLATION = 1;
static const int INDEX_TIME_1 = 2;
static const int INDEX_TIME_2 = 3;

/* Count of verbs, in a cache for optimization. */
static size_t cache_count_of_verbs = 0;

/* List of current results. */
LIST(current_results);

/**
 * Runs a simple statement that shall not fail and returns true.
 *
 * @param prepared The statement with all parameters ready.
 * @return True if success of query, False otherwise.
 */
static bool runSimpleQuery(sqlite3_stmt* prepared) {
    return
        (sqlite3_step(prepared) == SQLITE_DONE) &&
        (sqlite3_reset(prepared) == SQLITE_OK) &&
        (sqlite3_clear_bindings(prepared) == SQLITE_OK);
}

/**
 * Runs a statement that shall not fail.
 * Stores the results (in order) in the QueryResults.
 *
 * @param stmt The statement which shall be ready.
 * @return The object on which results will be written.
 */
static list_node* makeQueryResults(sqlite3_stmt *stmt) {
    int step_result = -1;
    list_init(current_results);

    while (step_result != SQLITE_DONE) {
        step_result = sqlite3_step(stmt);
        STRING infinitive = (STRING) sqlite3_column_text(stmt, INDEX_INFINITIVE);
        STRING translation = (STRING) sqlite3_column_text(stmt, INDEX_TRANSLATION);
        STRING time1 = (STRING) sqlite3_column_text(stmt, INDEX_TIME_1);
        STRING time2 = (STRING) sqlite3_column_text(stmt, INDEX_TIME_2);

        list_node current_node = {
                .next = NULL, /* Not necessary because already done by "list_add" but anyway */
                .verb = makeVerbFromStrings(
                        infinitive,
                        translation,
                        time1,
                        time2
        )};
        list_add(current_results, &current_node);
    }

    return (list_node *) current_results;
}

static void bindMultiStringsToStmt(sqlite3_stmt* stmt, int id, MultiStrings* multis) {
    STRING str = makeStringFromMultiStrings(multis);
    sqlite3_bind_text(stmt, id, str, -1, NULL);
    freeStringFromMultiStrings(str);
}

static void bindStringToStmt(sqlite3_stmt* stmt, int id, STRING to_bind) {
    sqlite3_bind_text(stmt, id, to_bind, -1, NULL);
}

static bool container_addVerb(const Verb* verb) {
    bindMultiStringsToStmt(sql_insert_verb, 1, verb->infinitive);
    bindMultiStringsToStmt(sql_insert_verb, 2, verb->translation);
    bindMultiStringsToStmt(sql_insert_verb, 3, verb->time1);
    bindMultiStringsToStmt(sql_insert_verb, 4, verb->time2);
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
        cache_count_of_verbs = 0;
        sqlite3_db_cacheflush(db);
        return results;
    } else return false;
}

list_node* container_getVerbsBySubstring(STRING str) {
    bindStringToStmt(sql_search, 1, str);
    return makeQueryResults(sql_search);
}

size_t container_getCount() {
    if (!cache_count_of_verbs) {
        runSimpleQuery(sql_count);
        cache_count_of_verbs = sqlite3_column_int(sql_count, 0);
        sqlite3_reset(sql_count);
    }
    return cache_count_of_verbs;
}

void container_freeResults() {
    while (current_results) {
        list_node* current_node = list_tail(current_results);
        freeVerb(current_node->verb);
        list_pop(current_results);
    }
}

void container_start_up() {
    if (sqlite3_open_v2("xyz.sqlite", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL) == SQLITE_OK) {
        STRING statement_text = "CREATE TABLE Verbs (infinitive TEXT PRIMARY KEY, translation TEXT, time1 TEXT, time2 TEXT) ;";
        if (sqlite3_exec(db, statement_text, NULL, NULL, NULL) == SQLITE_OK) {

            // Prepare all statements
            if (
                    (sqlite3_prepare_v2(db, sql_count_code, -1, &sql_count, NULL) == SQLITE_OK) &&
                    (sqlite3_prepare_v2(db, sql_insert_verb_code, -1, &sql_insert_verb, NULL) == SQLITE_OK) &&
                    (sqlite3_prepare_v2(db, sql_list_by_first_letter_code, -1, &sql_list_by_first_letter, NULL) == SQLITE_OK) &&
                    (sqlite3_prepare_v2(db, sql_search_code, -1, &sql_search, NULL) == SQLITE_OK))
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

STRING container_get_last_error() {
    return sqlite3_errmsg(db);
}
