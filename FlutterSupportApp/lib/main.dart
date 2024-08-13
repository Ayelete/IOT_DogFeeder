import 'package:flutter/material.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:flutter_local_notifications/flutter_local_notifications.dart';
import 'package:cloud_firestore/cloud_firestore.dart';
import 'home_page.dart';
import 'notification_service.dart';
import 'firestore_service.dart'; // Ajoutez cette ligne

final FlutterLocalNotificationsPlugin flutterLocalNotificationsPlugin = FlutterLocalNotificationsPlugin();
final GlobalKey<NavigatorState> navigatorKey = GlobalKey<NavigatorState>();

void main() async {
  // Initialize the widgets
  WidgetsFlutterBinding.ensureInitialized();

  // Initialize the Firebase database
  await Firebase.initializeApp();

  // Initialize the local notifications
  final NotificationService notificationService = NotificationService();
  await notificationService.initialize();

  // Initialize Firestore service
  final FirestoreService firestoreService = FirestoreService(notificationService);
  firestoreService.startListening(); // Commence à écouter les changements dans Firestore

  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Dog Feeder App',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      navigatorKey: navigatorKey,
      home: HomePage(),
      routes: {
        '/specificScreen': (context) => SpecificScreen(),
      },
    );
  }
}

class SpecificScreen extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Specific Screen'),
      ),
      body: Center(
        child: Text('This is the specific screen'),
      ),
    );
  }
}
