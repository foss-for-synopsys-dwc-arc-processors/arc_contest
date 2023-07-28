import React from 'react';
import { View, Text, Image, StyleSheet, TouchableOpacity } from 'react-native';

function Introduction({ navigation }) {
  return (
    <View style={styles.container}>
      <View style={styles.motivationContainer}>
        <Text style={styles.motivationTitle}>Motivation</Text>
        <Text style={styles.motivationText}>According to the Ministry of Transportation and Communications, the casualties of scooter riders are 20 times more than that of car drivers and others in 2022 in Taiwan.</Text>
        <Image source={require('./casualities.png')} style={styles.image} />
      </View>
      <View style={styles.promotionContainer}>
        <Text style={styles.promotionTitle}>Promotion</Text>
        <Text style={styles.promotionText}>WOLO - We Only Live Once - is aiming to analyze road conditions by deep learning. It can detect vehicles and their distance in the camera to alert users in real time.</Text>
        <Image source={require('./promotion.png')} style={styles.image} />
      </View>
      <TouchableOpacity style={styles.button} onPress={() => navigation.navigate('UserGuide')}>
        <Text style={styles.buttonText}>Go to User Guide</Text>
      </TouchableOpacity>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#fff',
    paddingHorizontal: 20,
    paddingVertical: 20,
  },
  motivationContainer: {
    marginBottom: 10,
  },
  motivationTitle: {
    fontSize: 20,
    fontWeight: 'bold',
    marginBottom: 5,
  },
  motivationText: {
    fontSize: 16,
    marginBottom: 5,
  },
  promotionContainer: {
    marginBottom: 10,
  },
  promotionTitle: {
    fontSize: 20,
    fontWeight: 'bold',
    marginBottom: 5,
  },
  promotionText: {
    fontSize: 16,
    marginBottom: 5,
  },
  image: {
    width: '100%',
    height: 150,
    resizeMode: 'contain',
  },
  button: {
    backgroundColor: '#4285F4',
    padding: 10,
    borderRadius: 5,
    marginTop: 10,
  },
  buttonText: {
    color: '#fff',
    fontWeight: 'bold',
    textAlign: 'center',
  },
});

export default Introduction;
