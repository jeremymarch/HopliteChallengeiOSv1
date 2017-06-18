/*
Compile:

export DYLD_LIBRARY_PATH=/usr/local/mysql/lib/
gcc -o importSqlite -L/usr/local/mysql/lib -I/usr/local/mysql/include newImportToSqlite.c -lsqlite3 -lmysqlclient

Description: Simply copies from mysql to sqlite for Greek AND Latin.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <mysql.h>

#define MYSQL_USER "root"
#define MYSQL_PASS "clam1234"
#define MYSQL_DB "jmarch_gr"
#define MYSQL_HOST "localhost"
#define MYSQL_GREEK_TABLE "perseusLSJ2"
#define MYSQL_LATIN_TABLE "perseusLS2"

//#define SQLITE_PATH "/Users/jeremy/Library/Application\ Support/iPhone\ Simulator/6.1/Applications/BA77D41A-7E35-4467-BDBB-CB14D04AA735/Documents/"
#define SQLITE_PATHold "/Users/jeremy/Dropbox/code/cocoa/philologus-v1.4WithWorkTowardSync/"
#define SQLITE_PATHold2 "/Users/jeremy/Library/Application Support/iPhone Simulator/7.1-64/Applications/F4581667-FE0C-4172-AD53-919A1A944DE6/Documents/"
#define SQLITE_PATH "/Users/jeremy/Dropbox/Code/cocoa/morph/morph/"

#define DEF_DB "morph.sqlite"
#define USERDATA_DB "userData.sqlite"

#define GREEK 1
#define LATIN 0
#define NUM_LANGUAGES 1 //so just greek for now.

#define WORDS 1		//Do word table?
#define DEFS 1 		//Do def table?

#define VERBS 1
#define ENDINGS 1

#define BATCH_SIZE 200

MYSQL *
connectMysql()
{
	MYSQL *conn;
	conn = mysql_init(NULL);
	if (conn == NULL) 
	{
		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
		return NULL;
	}
	if (mysql_real_connect(conn, MYSQL_HOST, MYSQL_USER, MYSQL_PASS, MYSQL_DB, 0, NULL, 0) == NULL) 
	{
		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
		return NULL;
	}
	if (mysql_query(conn, "SET NAMES UTF8;")) 
	{
		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
		return NULL;
	}
	printf("MySQL client version: %s\n", mysql_get_client_info());
	
	return conn;
}	

sqlite3 *
connectSqlite(char *path)
{
	sqlite3 *db;

	if (sqlite3_open(path, &db) != SQLITE_OK)
	{
		fprintf(stderr, "Can't open sqlite database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return NULL;
	}	 
	printf("SQLITE3 %s version: %s\n", path, sqlite3_libversion());
	
	return db;
}
	
int main(int argc, char **argv)
{
	MYSQL *conn;
	MYSQL_RES *result;
	MYSQL_ROW row;		 
	sqlite3 *db, *db2;
	char *zErrMsg = 0;
	sqlite3_stmt *statement;
	sqlite3_stmt *statement2;
	sqlite3_stmt *statement3;	
	char sqlitePrepquery[2024];
	char sqlitePrepquery2[1024];
	char sqlitePrepquery3[1024];
	char sqliteWordTable[20];
	char sqliteDefTable[20];
	char mysqlTable[20];	
	int lang;
	
	/*
	if (argc < 2)
	{
		printf("greek or latin?\n");
		exit(1);
	}
	
	if (!strncmp(argv[1], "greek", 5))
		lang = GREEK;
	else if (!strncmp(argv[1], "latin", 5))
		lang = LATIN;
	else
	{
		printf("greek or latin?\n");
		exit(1);
	}
	*/

	conn = connectMysql();
	db = 0;
	db = connectSqlite(SQLITE_PATH DEF_DB);
	//db2 = connectSqlite(SQLITE_PATH USERDATA_DB);
	
	if (!conn || !db)
	{
		printf("Could not connect to db.\n");
		exit(1);
	}

	//sqlite3_limit(db,SQLITE_LIMIT_VARIABLE_NUMBER,15);
	printf("Greek verbs: %d\n", SQLITE_LIMIT_VARIABLE_NUMBER);
	snprintf(mysqlTable, 19, MYSQL_GREEK_TABLE);

	snprintf(sqliteDefTable, 19, "ZVERBS");
	sqlite3_exec(db, "DELETE FROM ZVERBS", 0, 0, 0);
	//sqlite3_exec(db, "DROP INDEX ZGREEKDEFS_ZWORDID_INDEX", 0, 0, 0);

	snprintf(sqlitePrepquery, 2023, "INSERT INTO ZVERBS values (?,1,1,?,?,1,?,?,'',?,'',?,?,?)");	

	if (sqlite3_prepare_v2(db, sqlitePrepquery, strlen(sqlitePrepquery), &statement, NULL) != SQLITE_OK) 
	{
		printf("\nCould not prepare greek verbs statement: %s\n", sqlite3_errmsg(db));
		exit(1);
	}	

	char query[1024];
	snprintf(query, 1023, "select * from greek_verbs where hq > 0;");

	if (mysql_query(conn, query)) 
	{
		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
		exit(1);
	}

	result = mysql_store_result(conn);

	sqlite3_exec(db, "BEGIN", 0, 0, 0);

	int seq = 1;
	while ((row = mysql_fetch_row(result)))
	{
		int wordid = atoi(row[0]) + 1; //plus one because sqlite doesn't want pk to be 0
		if (row[0] == NULL)
		{
			printf("Row missing values: %s\n", row[0]);
			exit(1);
		}
	
		if (WORDS)
		{
			//pk
			printf(" : %d\n", wordid);
			
			if (sqlite3_bind_int(statement, 1, seq) != SQLITE_OK) 
			{
				printf("\nCould not bind id.\n");
				exit(1);
			}		
			//hq level
			if (sqlite3_bind_int(statement, 2, atoi(row[8])) != SQLITE_OK) 
			{
				printf("\nCould not bind wordid.\n");
				exit(1);
			}		
			//wordid
			if (sqlite3_bind_int(statement, 3, wordid) != SQLITE_OK) 
			{
				printf("\nCould not bind wordid.\n");
				exit(1);
			}/*
			//verbclass
			if (sqlite3_bind_int(statement, 4, atoi(row[9])) != SQLITE_OK) 
			{
				printf("\nCould not bind wordid.\n");
				exit(1);
			}	*/	
			//aorist
			if (sqlite3_bind_text(statement, 4, row[3], -1, SQLITE_STATIC) != SQLITE_OK) 
			{
				printf("\nCould not bind word.\n");
				exit(1);
			}			
			//aorist passive
			if (sqlite3_bind_text(statement, 5, row[6], -1, SQLITE_STATIC) != SQLITE_OK) 
			{
				printf("\nCould not bind word.\n");
				exit(1);
			}	/*
			//def
			if (sqlite3_bind_text(statement, 7, "", -1, SQLITE_STATIC) != SQLITE_OK) 
			{
				printf("\nCould not bind word.\n");
				exit(1);
			}*/
			//future
			if (sqlite3_bind_text(statement, 6, row[2], -1, SQLITE_STATIC) != SQLITE_OK) 
			{
				printf("\nCould not bind word.\n");
				exit(1);
			} /*
			//more
			if (sqlite3_bind_text(statement, 9, "", -1, SQLITE_STATIC) != SQLITE_OK) 
			{
				printf("\nCould not bind word.\n");
				exit(1);
			}	*/
			//perfect
			if (sqlite3_bind_text(statement, 7, row[4], -1, SQLITE_STATIC) != SQLITE_OK) 
			{
				printf("\nCould not bind word.\n");
				exit(1);
			}		
			//perfect mid
			if (sqlite3_bind_text(statement, 8, row[5], -1, SQLITE_STATIC) != SQLITE_OK) 
			{
				printf("\nCould not bind word.\n");
				exit(1);
			}																																	
			//present
			if (sqlite3_bind_text(statement, 9, row[1], -1, SQLITE_STATIC) != SQLITE_OK) 
			{
				printf("\nCould not bind unaccented_word.\n");
				exit(1);
			}
									
			if( sqlite3_step(statement) != SQLITE_DONE )
			{
				fprintf(stderr, "1SQL error: %s\nError: %s\n", zErrMsg, sqlite3_errmsg(db));
				sqlite3_free(zErrMsg);
				break;
			}
			sqlite3_reset(statement);	
		}

		seq++;
	}
	
/**********************/


	printf("Greek verb endings\n");
	snprintf(mysqlTable, 19, MYSQL_GREEK_TABLE);

	snprintf(sqliteWordTable, 19, "ZENDINGS");
	sqlite3_exec(db, "DELETE FROM ZENDINGS", 0, 0, 0);

	snprintf(sqlitePrepquery2 , 1023, "INSERT INTO ZENDINGS values (?,1,1,?,?,?,?,?,?,?,?,?,?,?)");	

	if (sqlite3_prepare_v2(db, sqlitePrepquery2, strlen(sqlitePrepquery2), &statement2, NULL) != SQLITE_OK) 
	{
		printf("\nCould not prepare statement2.\n");
		exit(1);
	}	

	char query2[1024];
	snprintf(query2, 1023, "select * from greek_endings;");

	if (mysql_query(conn, query2)) 
	{
		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
		exit(1);
	}

	result = mysql_store_result(conn);

	sqlite3_exec(db, "BEGIN", 0, 0, 0);

	seq = 1;
	while ((row = mysql_fetch_row(result)))
	{
		int wordid = atoi(row[0]) + 1; //plus one because sqlite doesn't want pk to be 0
		if (row[0] == NULL)
		{
			printf("Row missing values: %s\n", row[0]);
			exit(1);
		}
	
			//pk
			printf("abc: %d\n", wordid);
			
			if (sqlite3_bind_int(statement2, 1, seq) != SQLITE_OK) 
			{
				printf("\nCould not bind id.\n");
				exit(1);
			}
			//hq level
			if (sqlite3_bind_int(statement2, 2, atoi(row[10])) != SQLITE_OK) 
			{
				printf("\nCould not bind id.\n");
				exit(1);
			}				
			//endingid
			if (sqlite3_bind_int(statement2, 3, wordid) != SQLITE_OK) 
			{
				printf("\nCould not bind wordid.\n");
				exit(1);
			}					
			//tense
			if (sqlite3_bind_text(statement2, 9, row[1], -1, SQLITE_STATIC) != SQLITE_OK) 
			{
				printf("\nCould not bind unaccented_word.\n");
				exit(1);
			}
			//voice
			if (sqlite3_bind_text(statement2, 12, row[2], -1, SQLITE_STATIC) != SQLITE_OK) 
			{
				printf("\nCould not bind word.\n");
				exit(1);
			}
			//mood
			if (sqlite3_bind_text(statement2, 6, row[3], -1, SQLITE_STATIC) != SQLITE_OK) 
			{
				printf("\nCould not bind word.\n");
				exit(1);
			}
			//fs
			if (sqlite3_bind_text(statement2, 5, row[4], -1, SQLITE_STATIC) != SQLITE_OK) 
			{
				printf("\nCould not bind word.\n");
				exit(1);
			}
			//ss
			if (sqlite3_bind_text(statement2, 8, row[5], -1, SQLITE_STATIC) != SQLITE_OK) 
			{
				printf("\nCould not bind word.\n");
				exit(1);
			}
			//ts
			if (sqlite3_bind_text(statement2, 11, row[6], -1, SQLITE_STATIC) != SQLITE_OK) 
			{
				printf("\nCould not bind word.\n");
				exit(1);
			}
			//fp
			if (sqlite3_bind_text(statement2, 4, row[7], -1, SQLITE_STATIC) != SQLITE_OK) 
			{
				printf("\nCould not bind word.\n");
				exit(1);
			}
			//sp
			if (sqlite3_bind_text(statement2, 7, row[8], -1, SQLITE_STATIC) != SQLITE_OK) 
			{
				printf("\nCould not bind word.\n");
				exit(1);
			}
			//tp
			if (sqlite3_bind_text(statement2, 10, row[9], -1, SQLITE_STATIC) != SQLITE_OK) 
			{
				printf("\nCould not bind word.\n");
				exit(1);
			}																			
			if( sqlite3_step(statement2) != SQLITE_DONE )
			{
				fprintf(stderr, "1SQL error: %s\nError: %s\n", zErrMsg, sqlite3_errmsg(db));
				sqlite3_free(zErrMsg);
				break;
			}
			sqlite3_reset(statement2);	

		seq++;
	}
	
	//***********MENU**********
	sqlite3_exec(db, "DELETE FROM ZMENU", 0, 0, 0);						
	sqlite3_exec(db, "INSERT INTO zmenu VALUES (1,1,1,1,'Endings')", 0, 0, 0);
	sqlite3_exec(db, "INSERT INTO zmenu VALUES (2,1,1,2,'Principal Parts')", 0, 0, 0);
	sqlite3_exec(db, "INSERT INTO zmenu VALUES (3,1,1,3,'Accents')", 0, 0, 0);	
	sqlite3_exec(db, "INSERT INTO zmenu VALUES (4,1,1,4,'Morphology Training')", 0, 0, 0);	
	//sqlite3_exec(db, "INSERT INTO zmenu VALUES (5,1,1,5,'Syntax')", 0, 0, 0);	
	
	
	sqlite3_exec(db, "COMMIT", 0, 0, 0);	
	
	sqlite3_exec(db, "VACUUM;", 0, 0, 0);

	printf("Done\n");
 
	sqlite3_close(db);
		
	mysql_free_result(result); 
	mysql_close(conn);
	exit(0);
}
