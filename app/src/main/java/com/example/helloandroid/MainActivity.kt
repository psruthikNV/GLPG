package com.example.helloandroid

import android.app.Activity
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.content.Context
import android.content.Intent
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager
import android.os.Bundle
import android.util.Log
import android.widget.TextView
import android.widget.Toast


class MainActivity : Activity(), SensorEventListener {

    /* Declare required variables.
     * Kotlin requires an explicit
     * lateinit for this shit.*/
    private lateinit var sensorManager: SensorManager
    private lateinit var rotationVector: Sensor
    private var sensorArray = FloatArray(16)
    private lateinit var textViewList: Array<TextView?>
    private val btRequestCode = 1
    private var isBluetoothEnabled = RESULT_FIRST_USER
    private val TAG = "FUCK_LOG"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        sensorManager = getSystemService(Context.SENSOR_SERVICE) as SensorManager
        rotationVector = sensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR)
        sensorManager.registerListener(this, rotationVector, SensorManager.SENSOR_DELAY_FASTEST)
        textViewList = arrayOfNulls<TextView>(16)

        val textViewId = "textView"
        for (i in 0 until 16) {
            textViewList[i] = findViewById(resources.getIdentifier(textViewId + i.toString(),
                                                                    "id", packageName))
        }
        /* Initialize BT Stuff */
        val bluetoothAdapter: BluetoothAdapter? = BluetoothAdapter.getDefaultAdapter()
        if (bluetoothAdapter == null) {
            Toast.makeText(applicationContext, "Bluetooth is not supported on this device",
                           Toast.LENGTH_SHORT).show()
            finishAndRemoveTask()
        }
        if (bluetoothAdapter?.isEnabled == false) {
            val enableBtIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            startActivityForResult(enableBtIntent, btRequestCode)
        }

        val pairedDevices: Set<BluetoothDevice>? = bluetoothAdapter?.bondedDevices
        pairedDevices?.forEach { device ->
            val deviceName = device.name
            val deviceHardwareAddress = device.address
            Log.e(TAG, "Device name:" + device.name + " Device Address: " + device.address)
            if (device.address == "B8:31:B5:75:A5:DA") {
                Log.e(TAG, "Found laptop")
            }
        }
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        when (requestCode) {
            btRequestCode -> if (resultCode == RESULT_CANCELED) {
                Toast.makeText(applicationContext, "Disabling Bluetooth features",
                    Toast.LENGTH_SHORT).show()
                finishAndRemoveTask() // Fuck off if user does not enable BT
            }
        }
    }


    override fun onAccuracyChanged(sensor: Sensor, accuracy: Int) {
        //TODO - Is anything required here?
    }

    override fun onSensorChanged(event: SensorEvent?) {

        if (event != null && event.sensor.type == Sensor.TYPE_ROTATION_VECTOR) {
            SensorManager.getRotationMatrixFromVector(sensorArray, event.values)
            for (i in 0 until 16) {
                textViewList[i]?.text = sensorArray[i].toString()
            }
        }
    }
}
