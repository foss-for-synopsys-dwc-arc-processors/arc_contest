const express = require('express');
const bodyParser = require('body-parser');

const app = express();
app.use(bodyParser.json());

app.post('', (req, res) => {
  const postData = req.body;
  console.log('Received data:', postData);
  // Handle the received data as needed

  res.send('Data received successfully');
});

app.listen(8000, () => {
  console.log('Server is running on port 8080');
});
