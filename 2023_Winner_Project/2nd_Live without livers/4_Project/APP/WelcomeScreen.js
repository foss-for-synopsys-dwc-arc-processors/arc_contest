import React, { Component } from 'react';
import { View, Text, Image, StyleSheet, Button } from 'react-native';

class WelcomeScreen extends Component {
  render() {
    const { navigation } = this.props;
    return (
      <View style={styles.container}>
        <Image source={require('./wolo_icon.jpg')} style={styles.logo} />
        <Text style={styles.title}>Welcome to WOLO</Text>
        <Text style={styles.subtitle}>We Only Live Once!</Text>
        <View style={styles.buttonContainer}>
          <Button
            title="Get Started"
            onPress={() => navigation.navigate('Introduction')}
          />
        </View>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#fff',
  },
  logo: {
    width: 300,
    height: 300,
    marginBottom: 20,
  },
  title: {
    fontSize: 24,
    fontWeight: 'bold',
    textAlign: 'center',
  },
  subtitle: {
    fontSize: 18,
    marginBottom: 20,
    textAlign: 'center',
  },
  buttonContainer: {
    marginTop: 20,
    width: '80%',
  },
});

export default WelcomeScreen;
