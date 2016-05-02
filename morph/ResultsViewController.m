//
//  ResultsViewController.m
//  Hoplite Challenge
//
//  Created by Jeremy on 4/26/16.
//  Copyright © 2016 Jeremy March. All rights reserved.
//

#import "ResultsViewController.h"
#import "VerbSequence.h"
#import "HCResult.h"
#import "sqlite3.h"

@interface ResultsViewController ()

@end

extern sqlite3 *db;

NSMutableArray *results2;

@implementation ResultsViewController

int getVerbSeqCallback2(void *NotUsed, int argc, char **argv, char **azColName) {
    
    NotUsed = 0;
    int bufferLen = 1024;
    char buffer[bufferLen];
    VerbFormC vf;
        
    HCResult *a = [[HCResult alloc ]init];
    
    a.person = atoi(argv[0]);
    a.number = atoi(argv[1]);
    a.tense = atoi(argv[2]);
    a.voice = atoi(argv[3]);
    a.mood = atoi(argv[4]);
    a.verb = atoi(argv[5]);
    if (!argv[6])
        a.wrongAnswer = @"";
    else
        a.wrongAnswer = [NSString stringWithUTF8String: argv[6]];
    
    if (!argv[7])
        a.elapsedTime = @"";
    else
        a.elapsedTime = [NSString stringWithUTF8String:argv[7]];
    
    a.correct = atoi(argv[8]);
    
    vf.person = a.person;
    vf.number = a.number;
    vf.tense = a.tense;
    vf.voice = a.voice;
    vf.mood = a.mood;
    vf.verb = &verbs[a.verb];
    
    getForm(&vf, buffer, bufferLen, false, false);
    
    a.correctAnswer = [NSString stringWithUTF8String:buffer];
    
    getAbbrevDescription(&vf, buffer, bufferLen);
    
    a.stem = [NSString stringWithUTF8String:buffer];
    
    //a.stem = [NSString stringWithFormat:@"%@ %@ %@ %@ %@", [NSString stringWithUTF8String: persons[a.person]], [NSString stringWithUTF8String: numbers[a.number]],[NSString stringWithUTF8String: tenses[a.tense]], [NSString stringWithUTF8String: voices[a.voice]], [NSString stringWithUTF8String: moods[a.mood]]];
    [results2 addObject:a];

    return 0;
}

void getVerbSeq2()
{
    NSLog(@"query");
    char *err_msg = 0;
    [results2 removeAllObjects];
    int rc = sqlite3_exec(db, "SELECT person,number,tense,voice,mood,verbid,incorrectAns,elapsedtime,correct FROM verbseq ORDER BY ID DESC LIMIT 100;", getVerbSeqCallback2, 0, &err_msg);
}

- (void)viewDidLoad {
    [super viewDidLoad];
    [self.navigationController setNavigationBarHidden:NO];
    NSLog(@"results loaded");
    self.results = [[NSMutableArray alloc] initWithObjects:@"1st pl aor pass ind", @"3rd sing plup mid ind", @"2nd sing pres act opt", nil];
    
    results2 = [[NSMutableArray alloc] init];
    
    getVerbSeq2();
    
    // Uncomment the following line to preserve selection between presentations.
    // self.clearsSelectionOnViewWillAppear = NO;
    
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
#warning Incomplete implementation, return the number of sections
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
#warning Incomplete implementation, return the number of rows
    if (results2)
    {
        NSLog(@"results2 count: %lu", (unsigned long)[results2 count]);
        return [results2 count];
    }
    else
        return 0;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"ResultsCell" forIndexPath:indexPath];
    
    // Configure the cell...
    UILabel *lblName = (UILabel *)[cell viewWithTag:100];
    UIImageView *imageView = (UIImageView *)[cell viewWithTag:101];
    UILabel *yourAns = (UILabel *)[cell viewWithTag:102];
    UILabel *time = (UILabel *)[cell viewWithTag:103];
    UILabel *correctAns = (UILabel *)[cell viewWithTag:104];
    
    HCResult *h = [results2 objectAtIndex:[indexPath row]];
    
    NSString *imageName;
    if(h.correct)
        imageName = @"greencheck.png";
    else
        imageName = @"redx.png";
    [imageView setImage:[UIImage imageNamed:imageName]];
    
    [lblName setText:h.stem];
    [yourAns setText:h.wrongAnswer];
    [correctAns setText:h.correctAnswer];
    [time setText:h.elapsedTime];
    
    return cell;
}

/*
// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the specified item to be editable.
    return YES;
}
*/

/*
// Override to support editing the table view.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source
        [tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationFade];
    } else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }   
}
*/

/*
// Override to support rearranging the table view.
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath {
}
*/

/*
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the item to be re-orderable.
    return YES;
}
*/

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
