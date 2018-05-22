(function () {
    /*
     * Module
     */
    var Module = {};
	
    Module['onRuntimeInitialized'] = function () {
		console.log("onRuntimeInitialized received");
		window.stdlib.prepareCoreBindings();
		window.stdlib.run();
    }

    window.Module = Module

    /*
     * StdLib
     */
	function StdLib() {
	}
	
    StdLib.prototype.prepareCoreBindings = function () {
        this.coreBindings = {};
        this.coreBindings.RunTests = Module.cwrap('RunTests', '', '');
    }

	StdLib.prototype.run = function () {
		console.log("RunTest starting");
		this.coreBindings.RunTests();
		console.log("RunTest finished");
    }

    window.stdlib = new StdLib()
})()