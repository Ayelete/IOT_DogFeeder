import 'package:flutter/material.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:cloud_firestore/cloud_firestore.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await Firebase.initializeApp();
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: MyHomePage(),
    );
  }
}

class MyHomePage extends StatelessWidget {
  final FirebaseFirestore _firestore = FirebaseFirestore.instance;

  void _addData() {
    _firestore.collection('users').add({
      'name': 'John Doe',
      'email': 'john.doe@example.com',
    });
  }

  Widget _buildUserList(QuerySnapshot<Map<String, dynamic>> snapshot) {
    return ListView(
      children: snapshot.docs.map((DocumentSnapshot<Map<String, dynamic>> document) {
        return ListTile(
          title: Text(document.data()!['name']),
          subtitle: Text(document.data()!['email']),
        );
      }).toList(),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Flutter Firestore Example'),
      ),
      body: Column(
        children: <Widget>[
          ElevatedButton(
            onPressed: _addData,
            child: Text('Add Data'),
          ),
          Expanded(
            child: StreamBuilder<QuerySnapshot<Map<String, dynamic>>>(
              stream: _firestore.collection('users').snapshots(),
              builder: (context, AsyncSnapshot<QuerySnapshot<Map<String, dynamic>>> snapshot) {
                if (snapshot.connectionState == ConnectionState.waiting) {
                  return Center(child: CircularProgressIndicator());
                } else if (snapshot.hasError) {
                  return Center(child: Text('Error: ${snapshot.error}'));
                } else {
                  return _buildUserList(snapshot.data!);
                }
              },
            ),
          ),
        ],
      ),
    );
  }
}
