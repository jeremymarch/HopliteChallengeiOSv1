//
//  GreekForms.c
//  morph
//
//  Created by Jeremy on 4/28/15.
//  Copyright (c) 2015 Jeremy March. All rights reserved.
//

#include "GreekForms.h"
#include "libmorph.h"

Verb verbs[NUM_VERBS] = {
    { 0, CONSONANT_STEM_PERFECT_2, 1, "πέμπω", "πέμψω", "ἔπεμψα", "πέπομφα", "πέπεμμαι", "ἐπέμφθην" },
    { 1, 0, 1, "παιδεύω", "παιδεύσω", "ἐπαίδευσα", "πεπαίδευκα", "πεπαίδευμαι", "ἐπαιδεύθην" },
    { 2, 0, 2, "λῡ́ω", "λῡ́σω", "ἔλῡσα", "λέλυκα", "λέλυμαι", "ἐλύθην" },
    { 3, CONSONANT_STEM_PERFECT_4, 2, "κελεύω", "κελεύσω", "ἐκέλευσα", "κεκέλευκα", "κεκέλευσμαι", "ἐκελεύσθην" },
    { 4, CONSONANT_STEM_PERFECT_1, 3, "γράφω", "γράψω", "ἔγραψα", "γέγραφα", "γέγραμμαι", "ἐγράφην" },
    { 5, CONSONANT_STEM_PERFECT_3, 3, "φυλάττω", "φυλάξω", "ἐφύλαξα", "πεφύλακα", "πεφύλαγμαι", "ἐφυλάχθην" },
    { 6, 0, 3, "θῡ́ω", "θῡ́σω", "ἔθῡσα", "τέθυκα", "τέθυμαι", "ἐτύθην" },
    { 7, 0, 3, "παύω", "παύσω", "ἔπαυσα", "πέπαυκα", "πέπαυμαι", "ἐπαύθην" },
    { 8, CONSONANT_STEM_PERFECT_3, 4, "τάττω", "τάξω", "ἔταξα", "τέταχα", "τέταγμαι", "ἐτάχθην" },
    { 9, CONSONANT_STEM_PERFECT_1, 4, "θάπτω", "θάψω", "ἔθαψα", "", "τέθαμμαι", "ἐτάφην" },
    { 10, CONSONANT_STEM_PERFECT_3, 4, "διδάσκω", "διδάξω", "ἐδίδαξα", "δεδίδαχα", "δεδίδαγμαι", "ἐδιδάχθην" },
    { 11, 0, 4, "ἐθέλω", "ἐθελήσω", "ἠθέλησα", "ἠθέληκα", "", "" },
    { 12, CONSONANT_STEM_PERFECT_3, 5, "ἄρχω", "ἄρξω", "ἦρξα", "ἦρχα", "ἦργμαι", "ἤρχθην" },
    { 13, CONSONANT_STEM_PERFECT_1, 5, "βλάπτω", "βλάψω", "ἔβλαψα", "βέβλαφα", "βέβλαμμαι", "ἐβλάβην, ἐβλάφθην" },
    { 14, CONSONANT_STEM_PERFECT_4, 5, "πείθω", "πείσω", "ἔπεισα", "πέπεικα", "πέπεισμαι", "ἐπείσθην" }, //keep πέποιθα?
    { 15, CONSONANT_STEM_PERFECT_3, 5, "πρᾱ́ττω", "πρᾱ́ξω", "ἔπρᾱξα", "πέπρᾱχα, πέπρᾱγα", "πέπρᾱγμαι", "ἐπρᾱ́χθην" },
    { 16, 0, 6, "δουλεύω", "δουλεύσω", "ἐδούλευσα", "δεδούλευκα", "δεδούλευμαι", "ἐδουλεύθην" },
    { 17, 0, 6, "κωλῡ́ω", "κωλῡ́σω", "ἐκώλῡσα", "κεκώλῡκα", "κεκώλῡμαι", "ἐκωλῡ́θην" },
    { 18, 0, 6, "πολῑτεύω", "πολῑτεύσω", "ἐπολῑ́τευσα", "πεπολῑ́τευκα", "πεπολῑ́τευμαι", "ἐπολῑτεύθην" },
    { 19, 0, 6, "χορεύω", "χορεύσω", "ἐχόρευσα", "κεχόρευκα", "κεχόρευμαι", "ἐχορεύθην" },
    { 20, CONSONANT_STEM_PERFECT_1, 7, "λείπω", "λείψω", "ἔλιπον", "λέλοιπα", "λέλειμμαι", "ἐλείφθην" },
    { 21, CONSONANT_STEM_PERFECT_1, 7, "κλέπτω", "κλέψω", "ἔκλεψα", "κέκλοφα", "κέκλεμμαι", "ἐκλάπην" },
    { 22, CONSONANT_STEM_PERFECT_5, 7, "σῴζω", "σώσω", "ἔσωσα", "σέσωκα", "σέσωσμαι, σέσωμαι", "ἐσώθην" },
    { 23, CONSONANT_STEM_PERFECT_3, 8, "ἄγω", "ἄξω", "ἤγαγον", "ἦχα", "ἦγμαι", "ἤχθην" },
    { 24, 0, 8, "ἥκω", "ἥξω", "", "", "", "" },
    { 25, 0, 9, "νῑκάω", "νῑκήσω", "ἐνῑ́κησα", "νενῑ́κηκα", "νενῑ́κημαι", "ἐνῑκήθην" },
    { 26, 0, 9, "ἀδικέω", "ἀδικήσω", "ἠδίκησα", "ἠδίκηκα", "ἠδίκημαι", "ἠδικήθην" },
    { 27, 0, 9, "ποιέω", "ποιήσω", "ἐποίησα", "πεποίηκα", "πεποίημαι", "ἐποιήθην" },
    { 28, 0, 9, "τῑμάω", "τῑμήσω", "ἐτῑ́μησα", "τετῑ́μηκα", "τετῑ́μημαι", "ἐτῑμήθην" },
    { 29, 0, 10, "καλέω", "καλῶ", "ἐκάλεσα", "κέκληκα", "κέκλημαι", "ἐκλήθην" },
    { 30, 0, 10, "μένω", "μενῶ", "ἔμεινα", "μεμένηκα", "", "" },
    { 31, 0, 10, "ἀγγέλλω", "ἀγγελῶ", "ἤγγειλα", "ἤγγελκα", "ἤγγελμαι", "ἠγγέλθην" },
    { 32, 0, 10, "ἀξιόω", "ἀξιώσω", "ἠξίωσα", "ἠξίωκα", "ἠξίωμαι", "ἠξιώθην" },
    { 33, 0, 10, "δηλόω", "δηλώσω", "ἐδήλωσα", "δεδήλωκα", "δεδήλωμαι", "ἐδηλώθην" },
    { 34, 0, 10, "τελευτάω", "τελευτήσω", "ἐτελεύτησα", "τετελεύτηκα", "τετελεύτημαι", "ἐτελευτήθην" },
    { 35, 0, 11, "ἀκούω", "ἀκούσομαι", "ἤκουσα", "ἀκήκοα", "", "ἠκούσθην" },
    { 36, 0, 11, "βάλλω", "βαλῶ", "ἔβαλον", "βέβληκα", "βέβλημαι", "ἐβλήθην" },
    { 37, 0, 11, "βούλομαι", "βουλήσομαι", "", "", "βεβούλημαι", "ἐβουλήθην" },
    { 38, 0, 11, "λαμβάνω", "λήψομαι", "ἔλαβον", "εἴληφα", "εἴλημμαι", "ἐλήφθην" },
    { 39, 0, 11, "πάσχω", "πείσομαι", "ἔπαθον", "πέπονθα", "", "" },
    { 40, CONSONANT_STEM_PERFECT_3, 11, "δέχομαι", "δέξομαι", "ἐδεξάμην", "", "δέδεγμαι", "" },
    { 41, CONSONANT_STEM_PERFECT_3, 11, "ἀποδέχομαι", "ἀποδέξομαι", "ἀπεδεξάμην", "", "ἀποδέδεγμαι", "" },
    { 42, 0, 12, "δίδωμι", "δώσω", "ἔδωκα", "δέδωκα", "δέδομαι", "ἐδόθην" },
    { 43, 0, 12, "ἵστημι", "στήσω", "ἔστησα, ἔστην", "ἕστηκα", "ἕσταμαι", "ἐστάθην" },
    { 44, 0, 12, "τίθημι", "θήσω", "ἔθηκα", "τέθηκα", "τέθειμαι", "ἐτέθην" },
    { 45, 0, 12, "ἀποδίδωμι", "ἀποδώσω", "ἀπέδωκα", "ἀποδέδωκα", "ἀποδέδομαι", "ἀπεδόθην" },
    { 46, 0, 12, "ἀφίστημι", "ἀποστήσω", "ἀπέστησα, ἀπέστην", "ἀφέστηκα", "ἀφέσταμαι", "ἀπεστάθην" },
    { 47, 0, 12, "καθίστημι", "καταστήσω", "κατέστησα, κατέστην", "καθέστηκα", "καθέσταμαι", "κατεστάθην" },
    { 48, 0, 12, "φιλέω", "φιλήσω", "ἐφίλησα", "πεφίληκα", "πεφίλημαι", "ἐφιλήθην" },
    { 49, 0, 12, "φοβέομαι", "φοβήσομαι", "", "", "πεφόβημαι", "ἐφοβήθην" },
    { 50, 0, 13, "γίγνομαι", "γενήσομαι", "ἐγενόμην", "γέγονα", "γεγένημαι", "" },
    { 51, 0, 13, "ἔρχομαι", "ἐλεύσομαι", "ἦλθον", "ἐλήλυθα", "", "" },
    { 52, 0, 13, "μανθάνω", "μαθήσομαι", "ἔμαθον", "μεμάθηκα", "", "" },
    { 53, 0, 13, "μάχομαι", "μαχοῦμαι", "ἐμαχεσάμην", "", "μεμάχημαι", "" },
    { 54, 0, 13, "φεύγω", "φεύξομαι, φευξοῦμαι", "ἔφυγον", "πέφευγα", "", "" },
    { 55, 0, 13, "μεταδίδωμι", "μεταδώσω", "μετέδωκα", "μεταδέδωκα", "μεταδέδομαι", "μετεδόθην" },
    { 56, 0, 13, "μετανίσταμαι", "μεταναστήσομαι", "μετανέστην", "μετανέστηκα", "", "" },
    { 57, 0, 13, "μηχανάομαι", "μηχανήσομαι", "ἐμηχανησάμην", "", "μεμηχάνημαι", "" },
    { 58, 0, 14, "δείκνυμι", "δείξω", "ἔδειξα", "δέδειχα", "δέδειγμαι", "ἐδείχθην" },
    { 59, 0, 14, "λανθάνω", "λήσω", "ἔλαθον", "λέληθα", "", "" },
    { 60, 0, 14, "τυγχάνω", "τεύξομαι", "ἔτυχον", "τετύχηκα", "", "" },
    { 61, 0, 14, "χαίρω", "χαιρήσω", "", "κεχάρηκα", "", "ἐχάρην" },
    { 62, 0, 14, "ἐπιδείκνυμαι", "", "", "", "", "" },
    { 63, 0, 14, "ἐπανίσταμαι", "ἐπαναστήσομαι", "ἐπανέστην", "ἐπανέστηκα", "", "" },
    { 64, 0, 14, "ἐρωτάω", "ἐρωτήσω", "ἠρώτησα", "ἠρώτηκα", "ἠρώτημαι", "ἠρωτήθην" },
    { 65, 0, 14, "φθάνω", "φθήσομαι", "ἔφθασα, ἔφθην", "", "", "" },
    { 66, 0, 15, "αἱρέω", "αἱρήσω", "εἷλον", "ᾕρηκα", "ᾕρημαι", "ᾑρέθην" },
    { 67, 0, 15, "αἰσθάνομαι", "αἰθήσομαι", "ᾐσθόμην", "", "ᾔσθημαι", "" },
    { 68, 0, 15, "εἰμί", "ἔσομαι", "", "", "", "" },
    { 69, 0, 15, "ἕπομαι", "ἕψομαι", "ἑσπόμην", "", "", "" },
    { 70, 0, 15, "ὁράω", "ὄψομαι", "εἶδον", "ἑόρᾱκα, ἑώρᾱκα", "ἑώρᾱμαι, ὦμμαι", "ὤφθην" },
    { 71, 0, 15, "φέρω", "οἴσω", "ἤνεγκα, ἤνεγκον", "ἐνήνοχα", "ἐνήνεγμαι", "ἠνέχθην" },
    { 72, 0, 15, "διαφέρω", "διοίσω", "διήνεγκα, διήνεγκον", "διενήνοχα", "διενήνεγμαι", "διηνέχθην" },
    { 73, 0, 15, "συμφέρω", "συμοίσω", "συμήνεγκα, συμήνεγκον", "συμενήνοχα", "συμενήνεγμαι", "συμηνέχθην" },
    { 74, 0, 16, "βαίνω", "βήσομαι", "ἔβην", "βέβηκα", "", "" },
    { 75, 0, 16, "γιγνώσκω", "γνώσομαι", "ἔγνων", "ἔγνωκα", "ἔγνωσμαι", "ἐγνώσθην" },
    { 76, 0, 16, "λέγω", "ἐρῶ, λέξω", "εἶπον, ἔλεξα", "εἴρηκα", "εἴρημαι, λέλεγμαι", "ἐλέχθην, ἐρρήθην" },
    { 77, 0, 16, "νομίζω", "νομιῶ", "ἐνόμισα", "νενόμικα", "νενόμισμαι", "ἐνομίσθην" },
    { 78, 0, 16, "πίπτω", "πεσοῦμαι", "ἔπεσον", "πέπτωκα", "", "" },
    { 79, 0, 16, "φημί", "φήσω", "ἔφησα", "", "", "" },
    { 80, 0, 16, "ἀναβαίνω", "ἀναβήσομαι", "ἀνέβην", "ἀναβέβηκα", "", "" },
    { 81, 0, 16, "ἐκπίπτω", "ἐκπεσοῦμαι", "ἐξέπεσον", "ἐκπέπτωκα", "", "" },
    { 82, CONSONANT_STEM_PERFECT_3, 17, "δοκέω", "δόξω", "ἔδοξα", "", "δέδογμαι", "" },
    { 83, 0, 17, "ἐλαύνω", "ἐλῶ", "ἤλασα", "-ἐλήλακα", "ἐλήλαμαι", "ἠλάθην" },
    { 84, 0, 17, "ἔχω", "ἕξω, σχήσω", "ἔσχον", "ἔσχηκα", "-ἔσχημαι", "" },
    { 85, 0, 17, "ἁμαρτάνω", "ἁμαρτήσομαι", "ἥμαρτον", "ἡμάρτηκα", "ἡμάρτημαι", "ἡμαρτήθην" },
    { 86, 0, 17, "δύναμαι", "δυνήσομαι", "", "", "δεδύνημαι", "ἐδυνήθην" },
    { 87, 0, 17, "ἐπίσταμαι", "ἐπιστήσομαι", "", "", "", "ἠπιστήθην" },
    { 88, 0, 18, "ἀποθνήσκω", "ἀποθανοῦμαι", "ἀπέθανον", "τέθνηκα", "", "" },
    { 89, 0, 18, "ἀποκτείνω", "ἀποκτενῶ", "ἀπέκτεινα", "ἀπέκτονα", "", "" },
    { 90, 0, 18, "ἵημι", "ἥσω", "ἧκα", "εἷκα", "εἷμαι", "εἵθην" },
    { 91, 0, 18, "βουλεύω", "βουλεύσω", "ἐβούλευσα", "βεβούλευκα", "βεβούλευμαι", "ἐβουλεύθην" },
    { 92, 0, 18, "ζητέω", "ζητήσω", "ἐζήτησα", "ἐζήτηκα", "", "ἐζητήθην" },
    { 93, 0, 18, "ἀφῑ́ημι", "ἀφήσω", "ἀφῆκα", "ἀφεῖκα", "ἀφεῖμαι", "ἀφείθην" },
    { 94, 0, 18, "συνῑ́ημι", "συνήσω", "συνῆκα", "συνείκα", "συνεῖμαι", "συνείθην" },
    { 95, 0, 18, "μέλλω", "μελλήσω", "ἐμέλλησα", "", "", "" },
    { 96, 0, 18, "πιστεύω", "πιστεύσω", "ἐπίστευσα", "πεπίστευκα", "πεπίστευμαι", "ἐπιστεύθην" },
    { 97, 0, 19, "ἀπόλλυμι", "ἀπολῶ", "ἀπώλεσα, ἀπωλόμην", "ἀπολώλεκα, ἀπόλωλα", "", "" },
    { 98, 0, 19, "εὑρίσκω", "εὑρήσω", "ηὗρον, εὗρον", "ηὗρηκα, εὗρηκα", "ηὕρημαι", "ηὑρέθην" },
    { 99, 0, 19, "αἰσχῡ́νομαι", "αἰσχυνοῦμαι", "", "", "ᾔσχυμμαι", "ᾐσχύνθην" },
    { 100, 0, 19, "", "ἐρήσομαι", "ἠρόμην", "", "", "" },
    { 101, 0, 19, "ἡγέομαι", "ἡγήσομαι", "ἡγησάμην", "", "ἥγημαι", "ἡγήθην " },
    { 102, 0, 19, "κρῑ́νω", "κρινῶ", "ἔκρῑνα", "κέκρικα", "κέκριμαι", "ἐκρίθην" },
    { 103, 0, 19, "ἀποκρῑ́νομαι", "ἀποκρινοῦμαι", "ἀπεκρῑνάμην", "", "ἀποκέκριμαι", "" },
    { 104, 0, 19, "οἶδα", "εἴσομαι", "", "", "", "" },
    { 105, 0, 19, "σύνοιδα", "συνείσομαι", "", "", "", "" },
    { 106, 0, 20, "ἀφικνέομαι", "ἀφίξομαι", "ἀφῑκόμην", "", "ἀφῖγμαι", "" },
    { 107, 0, 20, "πυνθάνομαι", "πεύσομαι", "ἐπυθόμην", "", "πέπυσμαι", "" },
    { 108, CONSONANT_STEM_PERFECT_1, 20, "τρέπω", "τρέψω", "ἔτρεψα, ἐτραπόμην", "τέτροφα", "τέτραμμαι", "ἐτράμην, ἐτρέφθην" },
    { 109, CONSONANT_STEM_PERFECT_4, 20, "φαίνω", "φανῶ", "ἔφηνα", "πέφηνα, πέφαγκα", "πέφασμαι", "ἐφάνην, ἐφάνθην" },
    { 110, 0, 20, "δεῖ", "δεήσει", "ἐδέησε(ν)", "", "", "" },
    { 111, 0, 20, "κεῖμαι", "κείσομαι", "", "", "", "" },
    { 112, 0, 20, "χρή", "χρῆσται", "", "", "", "" }
};
