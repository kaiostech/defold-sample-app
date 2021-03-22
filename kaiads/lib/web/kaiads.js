
// https://kripken.github.io/emscripten-site/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html

var KaiAdsLibrary = {

    $KaiAdsState: {
        ad: null,
        activeElement: null,
     },

    $KaiAdsResultCodes: {
        PRELOAD_ERROR: 0,
        PRELOAD_OK: 1,
        SHOW_ERROR: 2,
        AD_DISPLAY: 3,
        AD_CLICK: 4,
        AD_CLOSE: 5,
    },

    KaiAds_PlatformPreload: function(cpublisher, cconfigjson, callback) {
         if (KaiAdsState.ad != null) {
            console.log("kaiads preload - ad already preloaded");
            {{{ makeDynCall('vii', 'callback') }}} (KaiAdsResultCodes.PRELOAD_OK, -1);
            return;
        }
        var publisher = UTF8ToString(cpublisher);
        var configjson = UTF8ToString(cconfigjson);
        var config = JSON.parse(configjson);

        if (config.container != null) {
            config.container = document.getElementById(config.container)
        }
        config.publisher = publisher;
        config.onerror = function(err) {
            console.error("kaiads onerror", err);
            {{{ makeDynCall('vii', 'callback') }}} (KaiAdsResultCodes.PRELOAD_ERROR, err);
        };
        config.onready = function(ad) {
            console.log("kaiads onready", ad);
            // user clicked the ad
            ad.on('click', () => {
                console.log("kaiads ad clicked");
                {{{ makeDynCall('vii', 'callback') }}} (KaiAdsResultCodes.AD_CLICK, -1);
            });

            // user closed the ad (currently only with fullscreen)
            ad.on('close', () => {
                console.log("kaiads ad closeed");
                KaiAdsState.activeElement.focus();
                {{{ makeDynCall('vii', 'callback') }}} (KaiAdsResultCodes.AD_CLOSE, -1);
            });

            // the ad succesfully displayed
            ad.on('display', () => {
                console.log("kaiads ad displayed");
                {{{ makeDynCall('vii', 'callback') }}} (KaiAdsResultCodes.AD_DISPLAY, -1);
            });

            KaiAdsState.ad = ad;
            {{{ makeDynCall('vii', 'callback') }}} (KaiAdsResultCodes.PRELOAD_OK, -1);
        };
        console.log("kaiads preload with config", config);
        getKaiAd(config);
    },

    KaiAds_PlatformShow: function(callback) {
         if (KaiAdsState.ad == null) {
            console.log("kaiads show - no ad preloaded");
            {{{ makeDynCall('vii', 'callback') }}} (KaiAdsResultCodes.SHOW_ERROR, -1);
            return;
        }
        console.log("kaiads ad display");
        KaiAdsState.activeElement = document.activeElement;
        KaiAdsState.ad.call('display');
        KaiAdsState.ad = null;
    }
}

autoAddDeps(KaiAdsLibrary, '$KaiAdsState');
autoAddDeps(KaiAdsLibrary, '$KaiAdsResultCodes');
mergeInto(LibraryManager.library, KaiAdsLibrary);
