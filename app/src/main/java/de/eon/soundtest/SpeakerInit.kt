package de.eon.soundtest

import android.content.ContentValues.TAG
import android.util.Log
import com.google.android.things.pio.Gpio
import com.google.android.things.pio.I2cDevice
import com.google.android.things.pio.PeripheralManager
import java.io.IOException

private const val TAG = "de.eon.soundtest.SpeakerInit"

internal interface I2cCmd {
	data class Data(
			val address: Int,
			val data: Byte
	) : I2cCmd

	data class DataBuf(
			val address: Int,
			val buf: ByteArray
	) : I2cCmd

	data class Delay(
			val ms: Long
	) : I2cCmd

	object Break : I2cCmd
}

private const val GPIO_FAULT = "GPIO3"
private const val GPIO_POWER_DOWN = "GPIO107"
private const val GPIO_MUTE = "GPIO108"

private const val I2C_ADDRESS_SPEAKER = 0x4C
private const val I2C_ADDRESS_MICROPHONE = 0x1B
private const val I2C_AUDIO_BUS = "I2C1"

fun initDsp() {
	Log.i("SpeakerInit", "Initializing speaker")

	val i2cSpeaker: I2cDevice
	val i2cMicrophone: I2cDevice

	PeripheralManager.getInstance().apply {
		openGpio(GPIO_FAULT).apply {
			setDirection(Gpio.DIRECTION_IN)
		}
		openGpio(GPIO_POWER_DOWN).apply {
			setActiveType(Gpio.ACTIVE_LOW)
			setDirection(Gpio.DIRECTION_OUT_INITIALLY_HIGH)
		}
		openGpio(GPIO_MUTE).apply {
			setActiveType(Gpio.ACTIVE_LOW)
			setDirection(Gpio.DIRECTION_OUT_INITIALLY_HIGH)
		}

		try {
			Thread.sleep(1000)
			i2cSpeaker = openI2cDevice(I2C_AUDIO_BUS, I2C_ADDRESS_SPEAKER)
//			i2cMicrophone = peripheralManager.openI2cDevice(I2C_AUDIO_BUS, I2C_ADDRESS_MICROPHONE)
		} catch (e: IOException) {
			Log.e(TAG, "Unable to open I2C devices", e)
			throw IllegalStateException(e)
		}
	}

	try {
		loop@ for (it in dspInitData) {
			when (it) {
				is I2cCmd.Data ->
					i2cSpeaker.writeRegByte(it.address, it.data)

				is I2cCmd.DataBuf ->
					i2cSpeaker.writeRegBuffer(it.address, it.buf, it.buf.size)

				is I2cCmd.Delay ->
					Thread.sleep(it.ms)

				is I2cCmd.Break ->
					break@loop
			}
		}

		Thread.sleep(1000)
	} catch (e: IOException) {
		Log.e("SpeakerInit", "Error writing bytes to speaker I2C device.", e)
	}

	Log.i("SpeakerInit", "Speaker init done")
}
