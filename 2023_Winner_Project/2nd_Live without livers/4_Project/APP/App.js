import React from 'react';
import { NavigationContainer } from '@react-navigation/native';
import { createStackNavigator } from '@react-navigation/stack';
import WelcomeScreen from './WelcomeScreen';
import Introduction from './Introduction';
import UserGuide from './UserGuide';
import Connection from './Connection';
import Activity from './Activity';

const Stack = createStackNavigator();

function App() {
  return (
    <NavigationContainer>
      <Stack.Navigator>
        <Stack.Screen
          name="Welcome"
          component={WelcomeScreen}
          options={{ title: 'Welcome to WOLO' }}
        />
        <Stack.Screen
          name="Introduction"
          component={Introduction}
          options={{ title: 'Introduction' }}
        />
        <Stack.Screen
          name="UserGuide"
          component={UserGuide}
          options={{ title: 'User Guide' }}
        />
        <Stack.Screen
          name="Connection"
          component={Connection}
          options={{ title: 'Connection' }}
        />
        <Stack.Screen
          name="Activity"
          component={Activity}
          options={{ title: 'Activity' }}
        />
      </Stack.Navigator>
    </NavigationContainer>
  );
}

export default App;
