const AudioContext = window.AudioContext || window.webkitAudioContext;

var defold = {
	audioCtx: new AudioContext(),
	audioBuffers: {},

	loadSoundFile: function(filepath) {
		return fetch(filepath).then(function(response) {
			return response.arrayBuffer();
		}).then(function(arrayBuffer) {
			return defold.audioCtx.decodeAudioData(arrayBuffer);
		}).catch(function(reason) {
			console.log(reason);
		});
	},

	playAudioBuffer: function(audioBuffer) {
		// check if context is in suspended state (autoplay policy)
		if (defold.audioCtx.state === 'suspended') {
			defold.audioCtx.resume();
		}
		const bufferSource = defold.audioCtx.createBufferSource();
		bufferSource.buffer = audioBuffer;
		bufferSource.connect(defold.audioCtx.destination)
		bufferSource.start();
		return bufferSource;
	},

	playAudioFromURL: function(url) {
		if (defold.audioBuffers[url]) {
			defold.playAudioBuffer(defold.audioBuffers[url]);
		}
		else {
			defold.loadSoundFile(url).then(function(audioBuffer) {
				defold.audioBuffers[url] = audioBuffer;
				defold.playAudioBuffer(defold.audioBuffers[url]);
			}).catch(function(reason) {
				console.log(reason);
			});
		}
	}
};
