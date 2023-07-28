import React from 'react';
import { View, Text, Button, StyleSheet, ScrollView } from 'react-native';

function UserGuide({ navigation }) {
  return (
    <View style={styles.container}>
      <ScrollView>
      <Text style={styles.title}>User Guide</Text>
      <Text style={styles.subtitle}>How to use WOLO</Text>
      <Text style={styles.paragraph}>
        WOLO is a mobile app designed to help scooter riders stay safe on the road. Here's how to use it:
      </Text>
      <Text style={styles.heading}>Step 1: Connect Your Camera</Text>
      <Text style={styles.paragraph}>
        To use WOLO, you need to connect your phone to a camera that can detect vehicles and their distance. Follow these steps to connect your camera:
      </Text>
      <Text style={styles.listItem}>
        1. Turn on your camera and make sure it's connected to Wi-Fi.
      </Text>
      <Text style={styles.listItem}>
        2. Open the WOLO app on your phone and tap the "Connect Camera" button.
      </Text>
      <Text style={styles.listItem}>
        3. Select your camera from the list of available devices.
      </Text>
      <Text style={styles.heading}>Step 2: Start Riding</Text>
      <Text style={styles.paragraph}>
        Once your camera is connected, you're ready to start using WOLO. Here's how to use it while riding your scooter:
      </Text>
      <Text style={styles.listItem}>
        1. Mount your camera to your scooter and turn it on.
      </Text>
      <Text style={styles.listItem}>
        2. Open the WOLO app on your phone and tap the "Start Riding" button.
      </Text>
      <Text style={styles.listItem}>
        3. Place your phone in a secure mount on your scooter so you can see the WOLO alerts while riding.
      </Text>
      <Button
        title="Go to Connection Page"
        onPress={() => navigation.navigate('Connection')}
      />
      </ScrollView>
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
      marginBottom: 10,
    },
    subtitle: {
      fontSize: 18,
      fontWeight: 'bold',
      marginBottom: 20,
    },
    paragraph: {
      fontSize: 16,
      marginBottom: 10,
    },
    heading: {
      fontSize: 18,
      fontWeight: 'bold',
      marginBottom: 10,
    },
    list: {
      marginLeft: 10,
      marginBottom: 10,
    },
    listItem: {
      flexDirection: 'row',
      marginBottom: 5,
    },
});

export default UserGuide;
