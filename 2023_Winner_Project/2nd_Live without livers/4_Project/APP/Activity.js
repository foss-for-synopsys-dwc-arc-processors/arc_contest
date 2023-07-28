import React, { useState, useEffect } from 'react';
import { StyleSheet, View, Text } from 'react-native';

const Activity = () => {
  const [objects, setObjects] = useState([]);
  // const [data, setData] = useState(null);

  // HTTP GET
  useEffect(() => {
    const fetchData = async () => {
      try {
        const response = await fetch('http://192.168.2.101:8001/'); // 替换为你的 API URL
        const json = await response.json();
        const obj_list = json.message.split('-');
        const newObjects = obj_list.map(item => {
          const [x, y] = item.split(',');
          const rx = 100 - (Math.floor(parseInt(x)/320.0 * 98) + 1);
          // const tmp = parseInt(y) > 15 ? 50 : 10;
          const tmp = parseInt(y);
          const ry = 100 - (Math.floor(tmp/100.0 * 98) + 1);
          return { rx, ry };
        });

        if(json.message == '-')
          setObjects([]);
        else
        // console.log(newObjects);
        setObjects(newObjects);

      } catch (error) {
        console.error(error);
      }
    };

    // 定时执行获取数据的操作
    const interval = setInterval(fetchData, 20);

    // 在组件卸载时清除定时器
    return () => {
      clearInterval(interval);
    };
  }, []);

  /*
  useEffect(() => {
    // a timer of 2second that repeat excecute
    const intervalId = setInterval(() => {
        const numObjects = Math.floor(Math.random() * 6);
        const newObjects = Array.from({ length: numObjects }, () => {
        const x = Math.floor(Math.random() * 98) + 1;
        const y = Math.floor(Math.random() * 98) + 1;
        return { x, y };
      });
      setObjects(newObjects);
    }, 2000);

    return () => clearInterval(intervalId);
  }, []);
  */
  return (
    <View style={styles.container}>
      <View style={styles.car}>
        <View style={styles.carBox} />
      </View>
      <View style={styles.line} />
      <View style={styles.bottomRegion}>
        {objects.map((obj, i) => (
          <View key={i} style={[styles.objectBox, { left: obj.rx + '%', bottom: obj.ry + '%' }]}>
            <View style={styles.objectDot} />
          </View>
        ))}
      </View>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'flex-start',
    backgroundColor: '#f5f5f5',
    paddingTop: '1%',
  },
  car: {
    width: 50,
    height: 100,
    justifyContent: 'center',
    alignItems: 'center',
    position: 'relative',
  },
  carBox: {
    width: 50,
    height: 100,
    backgroundColor: '#fff',
    borderRadius: 10,
    borderWidth: 4,
    borderColor: '#555',
  },
  line: {
    height: 2,
    width: '100%',
    backgroundColor: '#bbb',
    marginTop: 20,
    marginBottom: 20,
  },
  bottomRegion: {
    flex: 1,
    width: '100%',
    backgroundColor: '#fff',
    flexDirection: 'row',
    flexWrap: 'wrap',
    justifyContent: 'flex-start',
    alignItems: 'flex-start',
    paddingTop: 10,
    paddingBottom: 10,
    paddingHorizontal: 10,
  },
  objectBox: {
    position: 'absolute',
    width: 50,
    height: 75,
    borderWidth: 1,
    borderColor: 'red',
    justifyContent: 'center',
    alignItems: 'center',
    borderRadius: 10,
    backgroundColor: '#fff',
    shadowColor: '#000',
    shadowOffset: {
      width: 0,
      height: 2,
    },
    shadowOpacity: 0.23,
    shadowRadius: 2.62,

    elevation: 4,
  },
  objectDot: {
    width: 10,
    height: 10,
    backgroundColor: 'red',
    borderRadius: 5,
  },
});

export default Activity;
