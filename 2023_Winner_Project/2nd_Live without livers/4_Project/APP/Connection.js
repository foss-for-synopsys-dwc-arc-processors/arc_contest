import React from 'react';
import { View, Text, StyleSheet, Button } from 'react-native';

function Connection({ navigation }) {
  const handleStartPress = () => {
    // TODO: Add logic to handle "Start Riding" button press
    navigation.navigate('Activity');
  };

  return (
    <View style={styles.container}>
      <Text style={styles.title}>Connect to Wi-Fi</Text>
      <Text style={styles.paragraph}>
        Please connect your device to the Wi-Fi network with the SSID: "RW8300_11n_AP-B00C42".
      </Text>
      <Text style={styles.paragraph}>
        password: 27366939
      </Text>
      <Text style={styles.paragraph}>
        And set your IP : 192.168.2.102
      </Text>
      <Button
        title="Start Riding"
        onPress={handleStartPress}
      />
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#fff',
    paddingHorizontal: 20,
    paddingVertical: 40,
  },
  title: {
    fontSize: 24,
    fontWeight: 'bold',
    marginBottom: 20,
  },
  paragraph: {
    fontSize: 16,
    marginBottom: 20,
  },
});

export default Connection;
