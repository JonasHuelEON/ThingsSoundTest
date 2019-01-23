package de.eon.soundtest

import android.app.Activity
import android.content.Context
import android.media.*
import android.os.Build
import android.os.Bundle
import android.os.Handler
import android.util.Log
import java.io.DataInputStream
import kotlin.math.roundToInt


class MainActivity : Activity() {

	companion object {
		private val TAG = MainActivity::class.java.simpleName

		private val AUDIO_ATTRIBUTES_MUSIC = AudioAttributes.Builder()
				.setUsage(AudioAttributes.USAGE_MEDIA)
				.setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
				.build()
		private val AUDIO_FORMAT_OUT_MONO = AudioFormat.Builder()
				.setChannelMask(AudioFormat.CHANNEL_OUT_MONO)
				.setEncoding(AudioFormat.ENCODING_PCM_16BIT)
				.setSampleRate(48000)
				.build()
		private val AUDIO_FORMAT_OUT_STEREO = AudioFormat.Builder()
				.setChannelMask(AudioFormat.CHANNEL_OUT_STEREO)
				.setEncoding(AudioFormat.ENCODING_PCM_16BIT)
				.setSampleRate(48000)
				.build()
	}

	override fun onCreate(savedInstanceState: Bundle?) {
		super.onCreate(savedInstanceState)

		Log.d(TAG, "I'm alive!")

		listAudioDevices(AudioManager.GET_DEVICES_ALL)

		initDsp()

		Handler().postDelayed({
			playAudio1()
//		    playAudio2()
//			playAudio3()
//			volumeTest(0.8, 1.0)
		}, 2000)
	}

	private fun listAudioDevices(deviceFlag: Int) {
		val sb = StringBuilder()

		when (deviceFlag) {
			AudioManager.GET_DEVICES_OUTPUTS -> sb.append("List audio output devices").append(System.lineSeparator())
			AudioManager.GET_DEVICES_INPUTS -> sb.append("List audio input devices").append(System.lineSeparator())
			AudioManager.GET_DEVICES_ALL -> sb.append("List all audio devices").append(System.lineSeparator())
		}

		val manager = this.getSystemService(Context.AUDIO_SERVICE) as AudioManager
		val adis = manager.getDevices(deviceFlag)
		sb.append("Found ${adis.size} devices").append(System.lineSeparator())
		adis.forEachIndexed { i, adi ->
			sb.append("$i:").append(System.lineSeparator())
					.append("ProductName: ").append(adi.productName).append(System.lineSeparator())
					.append("Type: ").append(adi.type).append(System.lineSeparator())
					.append("IsSink: ").append(adi.isSink).append(System.lineSeparator())
					.append("IsSource: ").append(adi.isSource).append(System.lineSeparator())

			sb.append("ChannelMasks: ").append(System.lineSeparator())
			adi.channelMasks.forEach { sb.append(" $it").append(System.lineSeparator()) }

			sb.append("ChannelIndexMasks: ").append(System.lineSeparator())
			adi.channelIndexMasks.forEach { sb.append(" $it").append(System.lineSeparator()) }

			sb.append("Encodings: ").append(System.lineSeparator())
			adi.encodings.forEach { sb.append(" $it").append(System.lineSeparator()) }

			sb.append("SampleRates: ").append(System.lineSeparator())
			adi.sampleRates.forEach { sb.append(" $it").append(System.lineSeparator()) }

			sb.append("ChannelCounts: ").append(System.lineSeparator())
			adi.channelCounts.forEach { sb.append(" $it").append(System.lineSeparator()) }

			if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P)
				sb.append("Address: ").append(adi.address).append(System.lineSeparator())

			sb.append(System.lineSeparator())
		}

		Log.d(TAG, sb.toString())
	}

	private fun playAudio1() {
		Log.i(TAG, "PlayAudio1")

		val manager = this.getSystemService(Context.AUDIO_SERVICE) as AudioManager

		val maxVolume = manager.getStreamMaxVolume(AudioManager.STREAM_MUSIC)
		val targetVolume = (maxVolume * 1.0).roundToInt()
		Log.i(TAG, "setting volume to: $targetVolume")
		if (!manager.isVolumeFixed)
			manager.setStreamVolume(AudioManager.STREAM_MUSIC, targetVolume, 0)

		val mOutputBufferSize = AudioTrack.getMinBufferSize(
				AUDIO_FORMAT_OUT_STEREO.sampleRate,
				AUDIO_FORMAT_OUT_STEREO.channelMask,
				AUDIO_FORMAT_OUT_STEREO.encoding
		)

		val mAudioTrack = AudioTrack.Builder()
				.setAudioAttributes(AUDIO_ATTRIBUTES_MUSIC)
				.setAudioFormat(AUDIO_FORMAT_OUT_STEREO)
				.setBufferSizeInBytes(mOutputBufferSize)
				.build()

		val dis = DataInputStream(
				resources.openRawResource(
						R.raw.a440r48b16
				)
		)

		mAudioTrack.play()

		val bufferSize = 512
		val buffer = ByteArray(bufferSize)
		var i: Int

		while (true) {
			i = dis.read(buffer, 0, bufferSize)
			if (i == -1)
				break
			else
				mAudioTrack.write(buffer, 0, i)
		}

		dis.close()
		mAudioTrack.stop()

		Log.i(TAG, "PlayAudio1 track finished")

		Handler().postDelayed({
			playAudio1()
		}, 1000)
	}

	private fun playAudio2() {
		Log.i(TAG, "PlayAudio2")
		val mediaPlayer = MediaPlayer.create(applicationContext, R.raw.a440r48b16)
		mediaPlayer.setVolume(0.1f, 0.1f)
		mediaPlayer.setOnCompletionListener {
			Log.i(TAG, "PlayAudio2 track finished")
			it.release()
			Handler().postDelayed({
				playAudio2()
			}, 1000)
		}
		mediaPlayer.start()
	}

	private val arr0 = ByteArray(192) { 0 }
	private val arr127 = ByteArray(192) { 127 }
	private fun playAudio3() {
		Log.i(TAG, "PlayAudio3")

		val manager = this.getSystemService(Context.AUDIO_SERVICE) as AudioManager

		val maxVolume = manager.getStreamMaxVolume(AudioManager.STREAM_MUSIC)
		Log.i(TAG, "setting volume to: $maxVolume")
		if (!manager.isVolumeFixed)
			manager.setStreamVolume(AudioManager.STREAM_MUSIC, maxVolume, 0)

		val mOutputBufferSize = AudioTrack.getMinBufferSize(
				AUDIO_FORMAT_OUT_STEREO.sampleRate,
				AUDIO_FORMAT_OUT_STEREO.channelMask,
				AUDIO_FORMAT_OUT_STEREO.encoding
		)

		val mAudioTrack = AudioTrack.Builder()
				.setAudioAttributes(AUDIO_ATTRIBUTES_MUSIC)
				.setAudioFormat(AUDIO_FORMAT_OUT_STEREO)
				.setBufferSizeInBytes(mOutputBufferSize)
				.build()

		mAudioTrack.setVolume(AudioTrack.getMaxVolume())
		mAudioTrack.play()

		repeat(10000) {
			mAudioTrack.write(arr0, 0, arr0.size)
			mAudioTrack.write(arr127, 0, arr127.size)
		}

//		Thread.sleep(5000)
		mAudioTrack.stop()

		Log.i(TAG, "PlayAudio3 track finished")

		Handler().postDelayed({
			playAudio3()
		}, 1000)
	}

	private fun volumeTest(from: Double, to: Double) {
		Log.i(TAG, "VolumeTest")

		val manager = this.getSystemService(Context.AUDIO_SERVICE) as AudioManager

		val maxVolume = manager.getStreamMaxVolume(AudioManager.STREAM_MUSIC)

		val mOutputBufferSize = AudioTrack.getMinBufferSize(
				AUDIO_FORMAT_OUT_STEREO.sampleRate,
				AUDIO_FORMAT_OUT_STEREO.channelMask,
				AUDIO_FORMAT_OUT_STEREO.encoding
		)

		val mAudioTrack = AudioTrack.Builder()
				.setAudioAttributes(AUDIO_ATTRIBUTES_MUSIC)
				.setAudioFormat(AUDIO_FORMAT_OUT_STEREO)
				.setBufferSizeInBytes(mOutputBufferSize)
				.build()

		val dis = DataInputStream(
				resources.openRawResource(
						R.raw.a440r48b16
				)
		)

		mAudioTrack.play()

		val bufferSize = 100 * 1024
		val buffer = ByteArray(bufferSize)

		dis.read(buffer, 0, bufferSize).takeIf { it != -1 }?.also {
			println("BufferSize: $it")
			for (volume in (maxVolume*from).roundToInt() .. (maxVolume*to).roundToInt()) {
				if (!manager.isVolumeFixed) {
					println("volume: $volume")
					manager.setStreamVolume(AudioManager.STREAM_MUSIC, volume, 0)
				}

				mAudioTrack.write(buffer, 0, it)
			}
		}

		dis.close()
		mAudioTrack.stop()

		Log.i(TAG, "VolumeTest finished")

		Handler().postDelayed({
			volumeTest(from, to)
		}, 1000)
	}

}
