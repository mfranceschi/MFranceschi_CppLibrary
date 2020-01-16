//
// Created by mfran on 16/01/2020.
//

#include <stdio.h>
#include "VerbsContainer.h"
#include "SQLite/sqlite3.h"

static sqlite3* db;

static void makeQueryResults(QueryResults* output) {

}

bool addVerb(const Verb* verb);

QueryResults* getVerbsByInfinitive(STRING);
QueryResults* getVerbsByTranslation(STRING);
QueryResults* getVerbsByTime1(STRING);
QueryResults* getVerbsByTime2(STRING);

void freeQueryResults(QueryResults* qr) {
    for (size_t i = 0; i < qr->length; i++) {
        freeVerb(qr->results[i]);
    }
    free(qr->results);
    qr->results = NULL;
    qr->length = 0;
}

void startUp() {
    if (!sqlite3_open_v2(":memory:", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL)) {
        if (!sqlite3_exec(
                db,
                "CREATE TABLE Verbs (\n"
                "    infinitive TEXT PRIMARY KEY,\n"
                "    translation TEXT,\n"
                "    time1 TEXT,\n"
                "    time2 TEXT\n"
                ") ;",
                NULL,
                NULL,
                NULL))
        {
            printf("EXEC success!");
            printf("\n%s\n", sqlite3_errmsg(db));
            if (!sqlite3_exec(db, "insert into verbs values ('inf', 'tra', 'timun', 'timdeux');", NULL, NULL, NULL)) {
                printf("EXEC 2 success!");
                printf("\n%s\n", sqlite3_errmsg(db));
            } else {
                printf("EXEC 2 failed");
                printf("\n%s\n", sqlite3_errmsg(db));
            }
        } else {
            printf("EXEC failed!");
            shutDown();
        }

    } else {
        printf("OPEN Failed");
        printf("\n%s\n", sqlite3_errmsg(db));
    }
}

void shutDown() {
    printf("Shut Down");
    sqlite3_close(db);
}
