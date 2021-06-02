var KaiOSLibrary = {

    $Context: {
        listener: null
    },

    KaiOS_SetKeyboardListener: function(listener) {
        Context.listener = listener;

        document.activeElement.addEventListener("keydown", event => {
            console.log("keydown", event);
            {{{ makeDynCall("vii", "Context.listener") }}} (
                1,
                allocate(intArrayFromString(event.key), ALLOC_STACK)
            );
        }, { passive: false });

        document.activeElement.addEventListener("keyup", event => {
            console.log("keyup", event);
            {{{ makeDynCall("vii", "Context.listener") }}} (
                0,
                allocate(intArrayFromString(event.key), ALLOC_STACK)
            );
        }, { passive: false });
    }
}

autoAddDeps(KaiOSLibrary, "$Context");
mergeInto(LibraryManager.library, KaiOSLibrary);
