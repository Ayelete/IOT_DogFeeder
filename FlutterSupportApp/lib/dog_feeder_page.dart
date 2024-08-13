import 'package:flutter/material.dart';
import 'settings_page.dart';
import 'statistics_page.dart';  // Assurez-vous que cet import est correct

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
        StatisticsPage(dogId: dogId),  // Assurez-vous que cette classe est définie correctement
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
