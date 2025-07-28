#include "FirebaseManager.h"

// void tokenStatusCallback(TokenInfo info) {
//   Serial.printf("Token Info: type = %s, status = %s\n", 
//                 getTokenType(info.token_type).c_str(), 
//                 getTokenStatus(info.status).c_str());
// }

// Initialize Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void initFirebase() 
{
    // Set API key
    config.api_key = API_KEY;
   // config.project_id = FIREBASE_PROJECT_ID;
    
    // Authentication
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    //Assign the Firestore URL
   // config.database_url = "https://" FIREBASE_PROJECT_ID ".firebasedatabase.app"

    // //Token generation process indo
    // config.token_status_callback = tokenStatusCallback;
    
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
}
