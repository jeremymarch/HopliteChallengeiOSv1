//
//  VerbSequence.c
//  Hoplite Challenge
//
//  Created by Jeremy on 2/1/16.
//  Copyright © 2016 Jeremy March. All rights reserved.
//
#include <stdlib.h> // For random(), RAND_MAX
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "VerbSequence.h"
#include "sqlite3.h"

int findVerbIndexByPointer(Verb *v)
{
    for (int i = 0; i < NUM_VERBS; i++)
    {
        if (v == &verbs[i])
            return i;
    }
    return -1;
}

void randomAlternative(char *s, int *offset);

DataFormat *hcdata = NULL;
size_t sizeInBytes = 0;
const int sqlitePrepqueryLen = 1024;
char sqlitePrepquery[sqlitePrepqueryLen];
sqlite3_stmt *statement;
sqlite3_stmt *statement2;
sqlite3 *db;
int globalGameId = -1;

int getVerbSeqCallback(void *NotUsed, int argc, char **argv,
             char **azColName) {
    
    NotUsed = 0;
    
    for (int i = 0; i < argc; i++) {
        
        //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    
    //printf("\n");
    
    return 0;
}

void getVerbSeq()
{
    char *err_msg = 0;
    int rc = sqlite3_exec(db, "SELECT COUNT(*) FROM verbseq;", getVerbSeqCallback, 0, &err_msg);
}

//initialize to false since we're "changing" verbs on start anyway
//we don't want to rely on the back end code, so we have this variable
bool lastAnswerCorrect = false;

int callback(void *NotUsed, int argc, char **argv,
             char **azColName) {
    
    NotUsed = 0;
    
    for (int i = 0; i < argc; i++) {
        
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    
    printf("\n");
    
    return 0;
}

bool updateDB(VerbFormC *vf)
{
    char *zErrMsg = 0;
    
    int verbIndex = findVerbIndexByPointer(vf->verb);
    if (verbIndex < 0)
        return false;
    
    //if (sqlite3_bind_int(statement, 1, wordid) != SQLITE_OK) //id
    //    return false;
    long t = time(NULL);
    printf("Time: %s", ctime(&t));
    if (sqlite3_bind_int64(statement, 1, time(NULL)) != SQLITE_OK) //time
    {
        printf("2\n");
        return false;
    }
    if (sqlite3_bind_int(statement, 2, globalGameId) != SQLITE_OK) //gameid
    {
        printf("3\n");
        return false;
    }
    if (sqlite3_bind_int(statement, 3, vf->person) != SQLITE_OK) //person
    {
        printf("4\n");
        return false;
    }
    if (sqlite3_bind_int(statement, 4, vf->number) != SQLITE_OK) //num
    {
        printf("5\n");
        return false;
    }
    if (sqlite3_bind_int(statement, 5, vf->tense) != SQLITE_OK) //tense
    {
        printf("6\n");
        return false;
    }
    if (sqlite3_bind_int(statement, 6, vf->voice) != SQLITE_OK) //voice
    {
        printf("7\n");
        return false;
    }
    if (sqlite3_bind_int(statement, 7, vf->mood) != SQLITE_OK) //mood
    {
        printf("8\n");
        return false;
    }
    if (sqlite3_bind_int(statement, 8, verbIndex) != SQLITE_OK) //verbid
    {
        printf("9\n");
        return false;
    }
    if (sqlite3_bind_int(statement, 9, -1) != SQLITE_OK) //correct, set to invalid for now
    {
        printf("10\n");
        return false;
    }
    /*
    if (sqlite3_bind_text(statement, 11, text, -1, SQLITE_STATIC) != SQLITE_OK) //incorrect answer
    {
        printf("11\n");
        return false;
    }
*/
    sqlite3_exec(db, "BEGIN", 0, 0, 0);
    
    if( sqlite3_step(statement) != SQLITE_DONE )
    {
        fprintf(stderr, "1SQL error: %s\nError: %s\n", zErrMsg, sqlite3_errmsg(db));
        sqlite3_free(zErrMsg);
        return false;
    }
    
    sqlite3_exec(db, "COMMIT", 0, 0, 0);
    sqlite3_reset(statement);
    
    printf("updated db!\n");
    
    //char *err_msg = 0;
    //int rc = sqlite3_exec(db, "SELECT COUNT(*) FROM verbseq;", callback, 0, &err_msg);
    return true;
}

bool dbInit(const char *path)
{
    unsigned long dbpathLen = strlen(path) + 12;
    char dbpath[dbpathLen];
    snprintf(dbpath, dbpathLen - 1, "%sdb.sqlite", path);
    char *zErrMsg = 0;
    int rc = sqlite3_open(dbpath, &db);
    if( rc ){
        printf("Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }
    else
    {
        printf("SQLite OK\n");
    }
    
    //"DROP TABLE IF EXISTS games; DROP TABLE IF EXISTS verbseq;
    
    char *sql = "CREATE TABLE games (" \
    "gameid INTEGER PRIMARY KEY NOT NULL, " \
    "timest INT NOT NULL, " \
    "score INT NOT NULL " \
    "); " \
    
    "CREATE TABLE verbseq (" \
    "id INTEGER PRIMARY KEY NOT NULL, " \
    "timest INT NOT NULL, " \
    "gameid INT NOT NULL, " \
    "person INT1 NOT NULL, " \
    "number INT1 NOT NULL, " \
    "tense INT1 NOT NULL, " \
    "voice INT1 NOT NULL, " \
    "mood INT1 NOT NULL, " \
    "verbid INT NOT NULL, " \
    "correct INT1 NOT NULL, " \
    "elapsedtime VARCHAR(255), " \
    "incorrectAns VARCHAR(255), " \
    "FOREIGN KEY (gameid) REFERENCES games(gameid) " \
    "); " \
    
    "INSERT INTO games VALUES (1,0,-1);"; //This is the Practice Game
    
    rc = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
    if( rc!=SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    /*
    rc = sqlite3_exec(db, "INSERT INTO abc VALUES (1);", NULL, NULL, &zErrMsg);
    if( rc!=SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    */
    snprintf(sqlitePrepquery, sqlitePrepqueryLen, "INSERT INTO verbseq (timest,gameid,person,number,tense,voice,mood,verbid,correct,incorrectAns) VALUES (?,?,?,?,?,?,?,?,?,?)");
    if (sqlite3_prepare_v2(db, sqlitePrepquery, strlen(sqlitePrepquery), &statement, NULL) != SQLITE_OK)
    {
        printf("\nCould not prepare statement.\n");
        return false;
    }
    snprintf(sqlitePrepquery, sqlitePrepqueryLen, "UPDATE verbseq SET correct=?, elapsedtime=?, incorrectAns=? WHERE id=?;");
    if (sqlite3_prepare_v2(db, sqlitePrepquery, strlen(sqlitePrepquery), &statement2, NULL) != SQLITE_OK)
    {
        printf("\nCould not prepare statement2.\n");
        return false;
    }
    
    printf("sqlite success\n");
    
    return true;
}

void dataFileInit(const char* path)
{
    sizeInBytes = sizeof(DataFormat);//10*1024*1024; //10 mb
    
    printf("Data file in verbseq: %s\n", path);
    
    int fd = open(path, O_RDWR);
    
    struct stat st;
    
    fstat(fd, &st);
    size_t size = st.st_size;
    //printf("size: %zu\n", size);
    
    if (size < sizeInBytes) {
        off_t result = lseek(fd, sizeInBytes - 1, SEEK_SET);
        if (result == -1) {
            close(fd);
            printf("Error calling lseek() to 'stretch' the file");
            return;
        }
        result = write(fd, "", 1);
        if (result != 1) {
            close(fd);
            printf("Error writing last byte of the file");
            return;
        }
        
    }
    fstat(fd, &st);
    size = st.st_size;
    //printf("size: %zu\n", size);
    
    hcdata = mmap(NULL, sizeInBytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    if (hcdata == MAP_FAILED)
    {
        printf("MMap failed\n");
    } else {
        printf("MMap success\n");
    }
    
    close(fd); // we can close file now
}

void VerbSeqInit(const char *path)
{
    resetVerbSeq();
    
    //dataFileInit(path);
    //dbInit(path); //moved to appDelegate
}

void syncDataFile()
{
    if (hcdata)
    {
        msync(hcdata,sizeInBytes,MS_SYNC);
        printf("sync file\n");
    }
}

void closeDataFile()
{
    if (hcdata)
    {
        msync(hcdata,sizeInBytes,MS_SYNC);
        munmap(hcdata, sizeInBytes);
        printf("close file\n");
        hcdata = NULL;
    }
    if (db)
    {
        sqlite3_close(db);
    }
}

VerbFormRecord *getNextRecord()
{
    //VerbFormRecord *a = NULL;
    
    return &hcdata->vr[1];
}

VerbFormRecord *prevNextRecord()
{
    //VerbFormRecord *a = NULL;
    
    return &hcdata->vr[1];
}

void incrementHead()
{
    if (hcdata->head > 1000)
        hcdata->head = 0;
    else
        hcdata->head++;
}

bool setHeadAnswer(bool correct, char *answer, char *elapsedTime)
{
    lastAnswerCorrect = correct;

    time_t ltime; /* calendar time */
    ltime = time(NULL); /* get current cal time */
    
    if (hcdata)
    {
        hcdata->vr[hcdata->head].time = ltime;
        hcdata->vr[hcdata->head].correct = correct;
        unsigned long len = strlen(answer);
        strncpy(hcdata->vr[hcdata->head].answer, answer, (len > 199) ? 200 : len);
        
        //hcdata->vr[hcdata->head].answer = "222";
        incrementHead();
    }

    /*
    for (int i = 0; i < hcdata->head; i++)
    {
        printf("Rec: %s %s %s %s %s: %d %s, %s\n", persons[hcdata->vr[i].person], numbers[hcdata->vr[i].number], tenses[hcdata->vr[i].tense], voices[hcdata->vr[i].voice],moods[hcdata->vr[i].mood], hcdata->vr[i].correct, hcdata->vr[i].answer, asctime( localtime(&ltime) ));
    }
     */
    
    if (db)
    {
        char *zErrMsg = 0;
        int last_id = sqlite3_last_insert_rowid(db);
        
        if (sqlite3_bind_int(statement2, 1, correct) != SQLITE_OK) //correct
        {
            printf("2 1\n");
            return false;
        }
        /*if (sqlite3_bind_int(statement2, 2, 1) != SQLITE_OK) //time
        {
            printf("2 2\n");
            return false;
        }*/
        if (sqlite3_bind_text(statement2, 2, elapsedTime, -1, SQLITE_STATIC) != SQLITE_OK) //elapsed time
        {
            printf("2 2\n");
            return false;
        }
         if (sqlite3_bind_text(statement2, 3, answer, -1, SQLITE_STATIC) != SQLITE_OK) //incorrect answer
         {
         printf("2 3\n");
         return false;
         }
        if (sqlite3_bind_int(statement2, 4, last_id) != SQLITE_OK) //id
        {
            printf("2 4\n");
            return false;
        }
        sqlite3_exec(db, "BEGIN", 0, 0, 0);
        
        if( sqlite3_step(statement2) != SQLITE_DONE )
        {
            fprintf(stderr, "2 1SQL error: %s\nError: %s\n", zErrMsg, sqlite3_errmsg(db));
            sqlite3_free(zErrMsg);
            return false;
        }
        
        sqlite3_exec(db, "COMMIT", 0, 0, 0);
        sqlite3_reset(statement2);
        
        printf("updated db2!\n");
        
    }
    return true;
}

void setHead(VerbFormC *vf)
{
    printf("sethead\n");
    //VerbFormRecord *a = NULL;
    if (hcdata)
    {
        hcdata->vr[hcdata->head].person = vf->person;
        hcdata->vr[hcdata->head].number = vf->number;
        hcdata->vr[hcdata->head].tense = vf->tense;
        hcdata->vr[hcdata->head].voice = vf->voice;
        hcdata->vr[hcdata->head].mood = vf->mood;
        //hcdata->vr[hcdata->head].correct = correct;
        hcdata->vr[hcdata->head].verb = vf->verb;
    }
    if (db)
    {
        printf("setheaddb\n");
        updateDB(vf);
    }
}

bool compareFormsCheckMFRecordResult(UCS2 *expected, int expectedLen, UCS2 *given, int givenLen, bool MFPressed, char *elapsedTime)
{
    char buffer[200];
    bool ret = compareFormsCheckMF(expected, expectedLen, given, givenLen, MFPressed);
    
    ucs2_to_utf8_string(given, givenLen, (unsigned char*)buffer);
    
    setHeadAnswer(ret, buffer, elapsedTime);
    
    return ret;
}

static int verbSeq = 99999; //start more than repsPerVerb so we reset

void resetVerbSeq()
{
    lastAnswerCorrect = false;
    verbSeq = 99999;
}

long lastInitialDegreesToChange = 0;
int nextVerbSeq(int *seq, VerbFormC *vf1, VerbFormC *vf2, VerbSeqOptions *vso)
{
    static Verb *v;
    static Verb *lastV = NULL;
    static VerbFormC lastVF;
    
    int bufferLen = 2048;
    char buffer[bufferLen];
    long degreesToChange = vso->degreesToChange;
    
    if (!vso->isHCGame)
    {
        globalGameId = 1;
    }
    else
    {
        //create new game or use current game
        globalGameId = 1; //temp
    }
    if (vso->isHCGame)
    {
        if (!lastAnswerCorrect)
        {
            do //so we don't ask the same verb twice in a row
            {
                v = getRandomVerb(vso->units, vso->numUnits);
            } while (v == lastV);
            lastV = v;
            
            verbSeq = 1;
        }
        else
        {
            verbSeq++;
        }
    }
    else
    {
        if (verbSeq >= vso->repsPerVerb)
        {
            do //so we don't ask the same verb twice in a row
            {
                v = getRandomVerb(vso->units, vso->numUnits);
            } while (v == lastV);
            lastV = v;
            
            verbSeq = 1;
        }
        else
        {
            verbSeq++;
        }
    }
    
    *seq = verbSeq;
    
    //for testing on specific verbs, set here
    //vf1->verb = &verbs[48];
    vf1->verb = v;
    
    int highestUnit = 0;
    for (int i = 0; i < vso->numUnits; i++)
    {
        if (vso->units[i] > highestUnit)
            highestUnit = vso->units[i];
    }
    
    //only change 1 degree for unites 1 and 2
    if (highestUnit <= 2)
        degreesToChange = 1;
    
    if (vso->startOnFirstSing && verbSeq == 1)
    {
        vf1->person = FIRST;
        vf1->number = SINGULAR;
        vf1->tense = PRESENT;
        vf1->voice = ACTIVE;
        vf1->mood = INDICATIVE;
        
        //doesn't work if verb is deponent
        if (!getForm(vf1, buffer, bufferLen, false, false))
        {
            vf1->voice = MIDDLE;
            getForm(vf1, buffer, bufferLen, false, false); //do we need this?
        }
    }
    else if (verbSeq == 1)
    {
         do
         {
             generateForm(vf1);
         
         } while (!getForm(vf1, buffer, bufferLen, false, false) || !isValidFormForUnit(vf1, highestUnit) || !strncmp(buffer, "—", 1));
    }
    else
    {
        vf1->person = lastVF.person;
        vf1->number = lastVF.number;
        vf1->tense = lastVF.tense;
        vf1->voice = lastVF.voice;
        vf1->mood = lastVF.mood;
        vf1->verb = lastVF.verb;
        
        //we assume this is valid since it was the resulting form from last seq.
        //getForm(vf1, buffer, bufferLen, false, false);
    }

    do
    {
        if (verbSeq == 1)
        {
            int limit = 1000;
            do
            {
                /*
                if (highestUnit <= 2)
                    degreesToChange = 1;
                else
                    degreesToChange = randWithMax(4) + 2; //2-5
                */
                limit--;
                
            } while (degreesToChange == lastInitialDegreesToChange && limit > 0); //for variety
            /*
            if (limit == 0)
            {
                degreesToChange = 2;
            }
            
            lastInitialDegreesToChange = degreesToChange;
             */
        }
        
        //these need to be in the loop, so we're always starting from the same place
        vf2->person = vf1->person;
        vf2->number = vf1->number;
        vf2->tense = vf1->tense;
        vf2->voice = vf1->voice;
        vf2->mood = vf1->mood;
        vf2->verb = vf1->verb;
        
        changeFormByDegrees(vf2, degreesToChange);
    } while (!getForm(vf2, buffer, bufferLen, true, false) || !isValidFormForUnit(vf2, highestUnit) || !strncmp(buffer, "—", 1));
    
    lastVF.person = vf2->person;
    lastVF.number = vf2->number;
    lastVF.tense = vf2->tense;
    lastVF.voice = vf2->voice;
    lastVF.mood = vf2->mood;
    lastVF.verb = vf2->verb;
    
    /*
     //for testing to force form:
     vf2->person = SECOND;
     vf2->number = SINGULAR;
     vf2->tense = PRESENT;
     vf2->voice = PASSIVE;
     vf2->mood = INDICATIVE;
    vf2->verb = vf1->verb;
    */
    
    //temp
    if(verbSeq == 2 && vso->askPrincipalParts && !vso->isHCGame)
    {
        return VERB_SEQ_PP;
    }
    
    setHead(vf2); //we set it here, add whether it is correct later
    
    if (verbSeq == 1)
        return VERB_SEQ_CHANGE_NEW;
    else
        return VERB_SEQ_CHANGE;
}

/*
 This is because we don't want to ask to change mid to pass/pass to mid unless its aorist or future
 return true if the tense is not aorist or future
 and changing from mid to pass or pass to mid.
 false means it's ok to ask for this change, true means don't
 */
bool isMidToPassOrPassToMid(VerbFormC *vf, int tempTense, int tempVoice)
{
    //if the new tense is aorist or future then ok to change.
    if (tempTense == AORIST || tempTense == FUTURE)
        return false;
    
    if ((vf->voice == MIDDLE && tempVoice == PASSIVE) || (vf->voice == PASSIVE && tempVoice == MIDDLE))
    {
        printf("Hit mid to pass or pass to mid\n");
    
        return true;
    }
    else
    {
        return false;
    }
}

void changeFormByDegrees(VerbFormC *vf, int degrees)
{
    unsigned char tempPerson;
    unsigned char tempNumber;
    unsigned char tempTense;
    unsigned char tempVoice;
    unsigned char tempMood;
    
    int components[degrees];
    
    do
    {
        tempPerson = vf->person;
        tempNumber = vf->number;
        tempTense = vf->tense;
        tempVoice = vf->voice;
        tempMood = vf->mood;
        
        //re-initialize components array to invalid values
        for (int i = 0; i < degrees; i++)
            components[i] = -1;
        
        for (int i = 0; i < degrees; i++)
        {
            int n = (int)randWithMax(5);
            bool notAlreadyIn = true;
            for (int j = 0; j < degrees; j++)
            {
                if (n == components[j])
                {
                    notAlreadyIn = false;
                    --i;
                    break;
                }
            }
            if (notAlreadyIn)
                components[i] = n;
        }
        
        for (int i = 0; i < degrees; i++)
        {
            int v = components[i];
            
            if (v == PERSON)
            {
                tempPerson = incrementValue(NUM_PERSONS, vf->person);
            }
            else if (v == NUMBER)
            {
                tempNumber = incrementValue(NUM_NUMBERS, vf->number);
            }
            else if (v == TENSE)
            {
                tempTense = incrementValue(NUM_TENSES, vf->tense);
            }
            else if (v == VOICE)
            {
                tempVoice = incrementValue(NUM_VOICES, vf->voice);
            }
            else if (v == MOOD)
            {
                tempMood = incrementValue(NUM_MOODS, vf->mood);
            }
        }
    } //make sure form is valid and this verb has the required principal part,
    //and make sure we're not changing from mid to pass or vice versa unless the tense is aorist or future
    while (!formIsValidReal(tempPerson, tempNumber, tempTense, tempVoice, tempMood) || getPrincipalPartForTense(vf->verb, tempTense, tempVoice)[0] == '\0' || isMidToPassOrPassToMid(vf, tempTense, tempVoice));
    
    vf->person = tempPerson;
    vf->number = tempNumber;
    vf->tense = tempTense;
    vf->voice = tempVoice;
    vf->mood = tempMood;
}

//unit is the highest unit we're up to
bool isValidFormForUnit(VerbFormC *vf, int unit)
{
    if (unit <= 2)
    {
        if (vf->tense == PERFECT || vf->tense == PLUPERFECT || vf->voice != ACTIVE || vf->mood != INDICATIVE)
            return false;
    }
    else if (unit <= 4)
    {
        if (vf->voice != ACTIVE || vf->mood == IMPERATIVE)
            return false;
    }
    else if (unit <= 5)
    {
        if (vf->voice == MIDDLE || vf->mood == IMPERATIVE)
            return false;
    }
    else if (unit <= 7)
    {
        if (vf->mood == IMPERATIVE)
            return false;
    }
    else if (unit <= 11)
    {
        return true;
    }
    else if (unit <= 12)
    {
        if ((utf8HasSuffix(vf->verb->present, "μι") && vf->tense == AORIST) || (utf8HasSuffix(vf->verb->present, "στημι") && (vf->tense == AORIST || vf->tense == PERFECT || vf->tense == PLUPERFECT)))
            return false;
    }
    
    return true;
}

//sort with weights going from smallest to largest, they will be ints who add up to 100
//http://stackoverflow.com/questions/8529665/changing-probability-of-getting-a-random-number
int chooseRandomFromArrayWithWeighting(int *values, int len, int *weights)
{
    //or make weights a double? and out of 1?
    long rand = randWithMax(100);
    for (int i = 0; i < len; i++)
    {
        if (rand < weights[i])
            return values[i];
    }
    return 0; //shouldn't need this
}

void generateForm(VerbFormC *verbform)
{
    unsigned char iTense, iMood, iVoice, iPerson, iNumber;
    
    do
    {
        iTense = (unsigned char)randWithMax(NUM_TENSES);
        iMood = (unsigned char)randWithMax(NUM_MOODS);
        while ( iTense != PRESENT && iTense != AORIST && iMood != INDICATIVE )
            iMood = (unsigned char)randWithMax(NUM_MOODS);
        
        iVoice = (unsigned char)randWithMax(NUM_VOICES);
        /*
         if (iMood == 1)
         {
         iPerson = -1;
         iNumber = -1;
         }
         else if (iMood == 4)
         {
         iNumber = randWithMax([[self numbers] count]);
         iPerson = randWithMax([[self persons] count]);
         while (iPerson == 0)
         iPerson = randWithMax([[self persons] count]);
         }
         else
         {
         */
        iPerson = (unsigned char)randWithMax(NUM_PERSONS);
        iNumber = (unsigned char)randWithMax(NUM_NUMBERS);
        //}
        
        //NSArray conj = [NSArray arrayWithObjects: [NSNumber  v], nil];
        
        //NSUInteger randomIndex = randWithMax([theArray count]);
        //NSString *form = [NSString stringWithFormat:@"%@ %@ %@ %@ %@", (iPerson > -1) ? [[self persons] objectAtIndex: iPerson] : @"", (iNumber > -1) ? [[self numbers] objectAtIndex: iNumber] : @"", [[self tenses] objectAtIndex: iTense], [[self voices] objectAtIndex: iVoice], [[self moods] objectAtIndex: iMood]];
    }
    while (!formIsValidReal(iPerson, iNumber, iTense, iVoice, iMood) || getPrincipalPartForTense(verbform->verb, iTense, iVoice)[0] == '\0');
    
    verbform->person = iPerson;
    verbform->number = iNumber;
    verbform->tense = iTense;
    verbform->voice = iVoice;
    verbform->mood = iMood;
}

int incrementValue(int theArrayCount, int start)
{
    long n = randWithMax((theArrayCount - 1));
    for (int i = 0; i < n + 1; i++)
    {
        if (start < theArrayCount - 1)
            start++;
        else
            start = 0;
    }
    return start;
}

//From: http://stackoverflow.com/questions/2509679/how-to-generate-a-random-number-from-within-a-range
// Assumes 0 <= max <= RAND_MAX
// Returns in the half-open interval [0, max]
long randWithMax(unsigned int max)
{
    //return arc4random() % max;
    return arc4random_uniform(max);
    /*
     unsigned long
     // max <= RAND_MAX < ULONG_MAX, so this is okay.
     num_bins = (unsigned long) max + 1,
     num_rand = (unsigned long) RAND_MAX + 1,
     bin_size = num_rand / num_bins,
     defect   = num_rand % num_bins;
     
     long x;
     do {
     x = random();
     }
     // This is carefully written not to overflow
     while (num_rand - defect <= (unsigned long)x);
     
     // Truncated division is intentional
     return x/bin_size;
     */
}

//problem if match and distractor have are alternate forms of each other.
void getDistractorsForChange(VerbFormC *orig, VerbFormC *new, int numDistractors, char *buffer)
{
    VerbFormC vf;
    int i = 0;
    int n = 0;
    int starts[numDistractors + 1];
    int numStarts = 0;
    for (i = 0; i < numDistractors + 1; i++)
    {
        starts[i] = 0;
    }
    
    i = 0;
    char tempBuffer[2048];
    int offset = 0;
    
    getForm(new, tempBuffer, 2048, false, false); //put the changed form on the buffer so no duplicates
    randomAlternative(tempBuffer, &offset);
    strncpy(&buffer[n], &tempBuffer[offset], strlen(&tempBuffer[offset]));
    n += strlen(&tempBuffer[offset]);
    strncpy(&buffer[n], "; ", 2);
    n += 2;
    
    numStarts++;
    
    do
    {
        vf.verb = new->verb;
        vf.person = new->person;
        vf.number = new->number;
        vf.tense = new->tense;
        vf.voice = new->voice;
        vf.mood = new->mood;
        
        changeFormByDegrees(&vf, 1);
        
        getForm(&vf, tempBuffer, 2048, false, false);
        offset = 0;
        randomAlternative(tempBuffer, &offset);
        
        int j = 0;
        int noMatches = 1;
        for (j = 0; j < numStarts; j++)
        {
            if (memcmp(&tempBuffer[offset], &buffer[ starts[j] ], strlen(&tempBuffer[offset])) == 0 || memcmp(&tempBuffer[offset], "—", 1) == 0)
            {
                //printf("HEREREREREEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n");
                noMatches = 0;
                break;
            }
        }
        
        if (noMatches == 1)
        {
            //reduce alternate forms to just one
            strncpy(&buffer[n], &tempBuffer[offset], strlen(&tempBuffer[offset]));
            starts[numStarts] = n;
            numStarts++;
            
            //printf("%s\n", tempBuffer);
            n += strlen(&tempBuffer[offset]);
            strncpy(&buffer[n], "; ", 2);
            n += 2;
            
            i++;
        }
    } while (i < numDistractors);
    buffer[n - 2] = '\0';
}

void randomAlternative(char *s, int *offset)
{
    int starts[5] = { 0,0,0,0,0 };
    int numStarts = 1;
    unsigned long lenS = strlen(s);
    
    for (int i = 0; i < lenS; i++)
    {
        if (s[i] == ',')
        {
            starts[numStarts] = i + 2;
            numStarts++;
        }
    }
    long random = randWithMax(numStarts);
    *offset = starts[random];
    
    if (random < numStarts - 1)
        s[starts[random + 1] - 2] = '\0';
}


Verb *getRandomVerb(int *units, int numUnits)
{
    int u, v;
    int verbsToChooseFrom[NUM_VERBS];
    int numVerbsToChooseFrom = 0;
    for (v = 0; v < NUM_VERBS; v++)
    {
        for (u = 0; u < numUnits; u++)
        {
            if (verbs[v].hq == units[u])
            {
                verbsToChooseFrom[numVerbsToChooseFrom] = v;
                numVerbsToChooseFrom++;
                break;
            }
        }
    }
    int verb = (int)randWithMax(numVerbsToChooseFrom);
    return &verbs[ verbsToChooseFrom[verb] ];
}

Ending *getRandomEnding(int *units, int numUnits)
{
    int u, e;
    int endingsToChooseFrom[NUM_ENDINGS];
    int numEndingsToChooseFrom = 0;
    for (e = 0; e < NUM_ENDINGS; e++)
    {
        for (u = 0; u < numUnits; u++)
        {
            if (endings[e].hq == units[u])
            {
                endingsToChooseFrom[numEndingsToChooseFrom] = e;
                numEndingsToChooseFrom++;
                break;
            }
        }
    }
    int ending = (int)randWithMax(numEndingsToChooseFrom);
    return &endings[ endingsToChooseFrom[ending] ];
}

void getRandomEndingAsString(int *units, int numUnits, char *buffer, int bufferLen)
{
    //char description[512];
    Ending *e = getRandomEnding(units, numUnits);
    
    //endingGetDescription(1, description, 512);
    
    snprintf(buffer, bufferLen, "%s; %s; %s; %s; %s; %s; %s", e->description, e->fs, e->ss, e->ts, e->fp, e->sp, e->tp);
}
