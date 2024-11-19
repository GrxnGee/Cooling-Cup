package com.example.coolcup;

import androidx.appcompat.app.AppCompatActivity;

import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import org.json.JSONException;
import org.json.JSONObject;
import org.json.JSONTokener;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.Timer;
import java.util.TimerTask;

public class MainActivity extends AppCompatActivity {

    TextView temperatureView;

    private static final String SWITCH1_URL = "https://api.thingspeak.com/update?api_key=B1XLF4J8Q6WPOSMR&field4=";
    private static final String TEMPERATURE_URL = "https://api.thingspeak.com/channels/2256770/fields/1/last.json";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        temperatureView = findViewById(R.id.temp);

        new Timer().scheduleAtFixedRate(new TimerTask() {
            @Override
            public void run() {
                FetchThingspeakTask field1 = new FetchThingspeakTask();
                field1.execute(TEMPERATURE_URL);
            }
        }, 0, 3000);
    }







    public void onSwitch1Clicked(View v) {
        String FIELD4_URL;
        boolean on = ((Switch) v).isChecked();
        if (on) {
            FIELD4_URL = SWITCH1_URL + "1";
        } else {
            FIELD4_URL = SWITCH1_URL + "0";
        }
        FetchThingspeakTask field4 = new
                FetchThingspeakTask();
        field4.execute(FIELD4_URL);
    }




    private class FetchThingspeakTask extends AsyncTask<String, Void, String> {
        protected String doInBackground(String... urls) {
            try {
                URL url = new URL(urls[0]);
                HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();
                try {
                    BufferedReader bufferedReader = new BufferedReader(new
                            InputStreamReader(urlConnection.getInputStream()));
                    StringBuilder stringBuilder = new StringBuilder();
                    String line;
                    while ((line = bufferedReader.readLine()) != null) {
                        stringBuilder.append(line).append("\n");
                    }
                    bufferedReader.close();
                    return stringBuilder.toString();
                } finally {
                    urlConnection.disconnect();
                }
            } catch (Exception e) {
                Log.e("ERROR", e.getMessage(), e);
                return null;
            }
        }
        protected void onPostExecute(String response) {
            if (response == null) {
                // Handle the case where there was an error in the response
                Toast.makeText(MainActivity.this, "There was an error", Toast.LENGTH_SHORT).show();
                return;
            }

            try {
                Object jsonValue = new JSONTokener(response).nextValue();

                if (jsonValue instanceof JSONObject) {
                    JSONObject channel = (JSONObject) jsonValue;

                    if (channel.has("field1")) {
                        String message = getText(R.string.temp) + " " + channel.getString("field1") + " " + getText(R.string.Cel);
                        temperatureView.setText(message);
                    }

                    if (channel.has("field4")) {
                        String status = channel.getString("field4");
                        if (!status.equals("null")) {
                            String message;
                            if (status.equals("1")) {
                                message = getText(R.string.On) + " " + getText(R.string.Cool);
                            } else {
                                message = getText(R.string.Off) + " " + getText(R.string.Cool);
                            }
                            Toast.makeText(getApplicationContext(), message, Toast.LENGTH_SHORT).show();
                        }
                    }
                } else if (jsonValue instanceof Integer) {
                    int intValue = (Integer) jsonValue;
                    // Handle the case where the response is an integer
                    // You can add code here to handle this case appropriately
                } else {
                    // Handle other types of responses or unknown types here
                }
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }



    }


}