import 'package:flutter/material.dart';
import 'package:cloud_firestore/cloud_firestore.dart';
import 'add_dog_page.dart';  
import 'dog_feeder_page.dart';  

class HomePage extends StatelessWidget {
  final FirebaseFirestore _firestore = FirebaseFirestore.instance;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Welcome to the Dog App!'),
      ),
      body: Center(
        child: StreamBuilder(
          stream: _firestore.collection('Dogs').snapshots(),
          builder: (context, AsyncSnapshot<QuerySnapshot> snapshot) {
            if (snapshot.hasError) {
              return Text('Error: ${snapshot.error}');
            }
            switch (snapshot.connectionState) {
              case ConnectionState.waiting:
                return CircularProgressIndicator();
              default:
                if (snapshot.hasData) {
                  return Column(
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: snapshot.data!.docs.map((dog) {
                      return Padding(
                        padding: const EdgeInsets.all(8.0),
                        child: ElevatedButton(
                          style: ElevatedButton.styleFrom(
                            backgroundColor: Colors.blue, 
                            foregroundColor: Colors.white, // Couleur du texte
                            shape: RoundedRectangleBorder(
                              borderRadius: BorderRadius.circular(30.0),
            ),
  padding: EdgeInsets.symmetric(horizontal: 50, vertical: 20),
),
                          onPressed: () {
                            Navigator.push(
                              context,
                              MaterialPageRoute(builder: (context) => DogFeederPage(dogId: dog.id)),
                            );
                          },
                          child: Text("${dog.id}'s dog feeder"),
                        ),
                      );
                    }).toList()
                      ..add(
                        Padding(
                          padding: const EdgeInsets.all(8.0),
                          child: ElevatedButton(
                            style: ElevatedButton.styleFrom(
                              backgroundColor: Colors.lightBlueAccent.shade100, // Lighter background color for add button
                              foregroundColor: Colors.white, // Text color
                              shape: RoundedRectangleBorder(
                                borderRadius: BorderRadius.circular(30.0),
                              ),
                              padding: EdgeInsets.symmetric(horizontal: 50, vertical: 20),
                            ),
                            onPressed: () {
                              Navigator.push(
                                context,
                                MaterialPageRoute(builder: (context) => AddDogPage()),
                              );
                            },
                            child: Row(
                              mainAxisSize: MainAxisSize.min, // To minimize the button's width
                              children: [
                                Icon(Icons.add, color: Colors.white),
                                Text(' Add a new dog'),
                              ],
                            ),
                          ),
                        ),
                      ),
                  );
                } else {
                  return Text('No dogs found');
                }
            }
          },
        ),
      ),
    );
  }
}