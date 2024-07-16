import 'package:flutter/material.dart';
import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:intl/intl.dart';

class DogFeederPage extends StatefulWidget {
  final String dogId;

  DogFeederPage({required this.dogId});

  @override
  _DogFeederPageState createState() => _DogFeederPageState();
}

class _DogFeederPageState extends State<DogFeederPage> {
  int _selectedIndex = 0;

  List<Widget> _widgetOptions(String dogId) => [
        SettingsPage(dogId: dogId),
        StatisticsPage(dogId: dogId),
      ];

  void _onItemTapped(int index) {
    setState(() {
      _selectedIndex = index;
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(
          'Welcome to ${widget.dogId}\'s dog feeder',
          style: TextStyle(
            fontSize: 18,
          ),
        ),
        leading: IconButton(
          icon: Icon(Icons.arrow_back),
          onPressed: () => Navigator.pop(context),
        ),
      ),
      body: Center(
        child: _widgetOptions(widget.dogId)[_selectedIndex],
      ),
      bottomNavigationBar: BottomNavigationBar(
        items: const <BottomNavigationBarItem>[
          BottomNavigationBarItem(
            icon: Icon(Icons.settings),
            label: 'Settings',
          ),
          BottomNavigationBarItem(
            icon: Icon(Icons.bar_chart),
            label: 'Statistics',
          ),
        ],
        currentIndex: _selectedIndex,
        onTap: _onItemTapped,
      ),
    );
  }
}

class SettingsPage extends StatefulWidget {
  final String dogId;

  SettingsPage({required this.dogId});

  @override
  _SettingsPageState createState() => _SettingsPageState();
}

class _SettingsPageState extends State<SettingsPage> {
  final FirebaseFirestore _firestore = FirebaseFirestore.instance;

  Future<void> _addOrEditMeal({String? mealId, double? amountOfFood, TimeOfDay? hourOfMeal}) async {
  final mealsCollection = _firestore.collection('Dogs').doc(widget.dogId).collection('meals');
  final mealsSnapshot = await mealsCollection.get();
  final mealCount = mealsSnapshot.docs.length;

  if (mealId == null && mealCount >= 5) {
    showDialog<void>(
      context: context,
      builder: (BuildContext context) {
        return AlertDialog(
          title: Text('Error'),
          content: SingleChildScrollView(
            child: ListBody(
              children: <Widget>[
                Text('You cannot add more than 5 meals.'),
              ],
            ),
          ),
          actions: <Widget>[
            TextButton(
              child: Text('OK'),
              onPressed: () {
                Navigator.of(context).pop();
              },
            ),
          ],
        );
      },
    );
    return;
  }

  double _amountOfFood = amountOfFood ?? 100.0;
  TimeOfDay _selectedTime = hourOfMeal ?? TimeOfDay.now();

  String documentName;
  if (mealId == null) {
    documentName = 'Meal ${mealCount + 1}';
  } else {
    documentName = mealId; 
  }

  return showDialog<void>(
    context: context,
    barrierDismissible: false,
    builder: (BuildContext context) {
      return StatefulBuilder(
        builder: (context, setState) {
          return AlertDialog(
            title: Text(mealId == null ? 'Add Meal' : 'Edit Meal'),
            content: SingleChildScrollView(
              child: ListBody(
                children: <Widget>[
                  Text("Amount of Food: ${_amountOfFood.toInt()}g"),
                  Slider(
                    value: _amountOfFood,
                    min: 100,
                    max: 1000,
                    divisions: 9,
                    label: _amountOfFood.round().toString(),
                    onChanged: (value) {
                      setState(() {
                        _amountOfFood = value;
                      });
                    },
                  ),
                  Row(
                    mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                    children: <Widget>[
                      Expanded(
                        child: DropdownButton<int>(
                          isExpanded: true,
                          value: _selectedTime.hour,
                          onChanged: (int? newValue) {
                            setState(() {
                              _selectedTime = TimeOfDay(hour: newValue!, minute: _selectedTime.minute);
                            });
                          },
                          items: List.generate(24, (index) => DropdownMenuItem(
                            value: index,
                            child: Text("$index"),
                          )),
                        ),
                      ),
                      Expanded(
                        child: DropdownButton<int>(
                          isExpanded: true,
                          value: _selectedTime.minute,
                          onChanged: (int? newValue) {
                            setState(() {
                              _selectedTime = TimeOfDay(hour: _selectedTime.hour, minute: newValue!);
                            });
                          },
                          items: List.generate(60, (index) => DropdownMenuItem(
                            value: index,
                            child: Text("$index"),
                          )),
                        ),
                      ),
                    ],
                  ),
                ],
              ),
            ),
            actions: <Widget>[
              TextButton(
                child: Text('Cancel'),
                onPressed: () {
                  Navigator.of(context).pop();
                },
              ),
              TextButton(
                child: Text('Save'),
                onPressed: () async {
                  String timeString = '${_selectedTime.hour.toString().padLeft(2, '0')}:${_selectedTime.minute.toString().padLeft(2, '0')}';

                  if (mealId == null) 
                  {
                    await mealsCollection.doc(documentName).set({
                    'amountOfFood': _amountOfFood,
                    'hourOfMeal': '${_selectedTime.hour.toString().padLeft(2, '0')}:${_selectedTime.minute.toString().padLeft(2, '0')}',
                    });
                  } else {
                    await mealsCollection.doc(documentName).update({
                    'amountOfFood': _amountOfFood,
                    'hourOfMeal': '${_selectedTime.hour.toString().padLeft(2, '0')}:${_selectedTime.minute.toString().padLeft(2, '0')}',
                  });
                }
                  Navigator.of(context).pop();
                },
              ),
            ],
          );
        },
      );
    },
  );
}

  Future<void> _deleteMeal(String mealId) async {
    await _firestore.collection('Dogs').doc(widget.dogId).collection('meals').doc(mealId).delete();
  }

  @override
Widget build(BuildContext context) {
  return Scaffold(
    body: Column(
      mainAxisAlignment: MainAxisAlignment.start,
      children: <Widget>[
        Padding(
          padding: const EdgeInsets.all(16.0),
          child: Text(
            'Settings',
            style: TextStyle(
              fontSize: 24.0,
              fontWeight: FontWeight.bold,
            ),
            textAlign: TextAlign.center,
          ),
        ),
        Expanded(
          child: StreamBuilder<QuerySnapshot>(
            stream: _firestore.collection('Dogs').doc(widget.dogId).collection('meals').snapshots(),
            builder: (context, snapshot) {
              if (!snapshot.hasData) return CircularProgressIndicator();
              var meals = snapshot.data!.docs;
              return ListView.builder(
                itemCount: meals.length,
                itemBuilder: (context, index) {
                  var meal = meals[index];
                  return ListTile(
                    title: Text('Meal ${index + 1}'),
                    subtitle: Text('Amount: ${meal['amountOfFood']}g, \nHour: ${meal['hourOfMeal']}'),
                    trailing: Row(
                      mainAxisSize: MainAxisSize.min,
                      children: [
                        IconButton(
                          icon: Icon(Icons.edit),
                          onPressed: () {
                            final DateTime parsedTime = DateFormat('HH:mm').parse(meal['hourOfMeal']);
                            final TimeOfDay timeOfDay = TimeOfDay(hour: parsedTime.hour, minute: parsedTime.minute);

  _addOrEditMeal(
    mealId: meal.id,
    amountOfFood: meal['amountOfFood'].toDouble(), // Ensure this is a double
    hourOfMeal: timeOfDay,
  );
},
                        ),
                        IconButton(
                          icon: Icon(Icons.delete),
                          onPressed: () => _deleteMeal(meal.id),
                        ),
                      ],
                    ),
                  );
                },
              );
            },
          ),
        ),
      ],
    ),
    floatingActionButton: FloatingActionButton(
      onPressed: () => _addOrEditMeal(),
      child: Icon(Icons.add),
    ),
  );
}
}

class StatisticsPage extends StatelessWidget {
  final String dogId;

  StatisticsPage({required this.dogId});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Center(
        child: Text('Statistics for $dogId'),
      ),
    );
  }
}