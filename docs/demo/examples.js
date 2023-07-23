
  var Module = typeof Module !== 'undefined' ? Module : {};

  if (!Module.expectedDataFileDownloads) {
    Module.expectedDataFileDownloads = 0;
  }

  Module.expectedDataFileDownloads++;
  (function() {
    // Do not attempt to redownload the virtual filesystem data when in a pthread or a Wasm Worker context.
    if (Module['ENVIRONMENT_IS_PTHREAD'] || Module['$ww']) return;
    var loadPackage = function(metadata) {

      var PACKAGE_PATH = '';
      if (typeof window === 'object') {
        PACKAGE_PATH = window['encodeURIComponent'](window.location.pathname.toString().substring(0, window.location.pathname.toString().lastIndexOf('/')) + '/');
      } else if (typeof process === 'undefined' && typeof location !== 'undefined') {
        // web worker
        PACKAGE_PATH = encodeURIComponent(location.pathname.toString().substring(0, location.pathname.toString().lastIndexOf('/')) + '/');
      }
      var PACKAGE_NAME = 'docs/demo/examples.data';
      var REMOTE_PACKAGE_BASE = 'examples.data';
      if (typeof Module['locateFilePackage'] === 'function' && !Module['locateFile']) {
        Module['locateFile'] = Module['locateFilePackage'];
        err('warning: you defined Module.locateFilePackage, that has been renamed to Module.locateFile (using your locateFilePackage for now)');
      }
      var REMOTE_PACKAGE_NAME = Module['locateFile'] ? Module['locateFile'](REMOTE_PACKAGE_BASE, '') : REMOTE_PACKAGE_BASE;
var REMOTE_PACKAGE_SIZE = metadata['remote_package_size'];

      function fetchRemotePackage(packageName, packageSize, callback, errback) {
        
        var xhr = new XMLHttpRequest();
        xhr.open('GET', packageName, true);
        xhr.responseType = 'arraybuffer';
        xhr.onprogress = function(event) {
          var url = packageName;
          var size = packageSize;
          if (event.total) size = event.total;
          if (event.loaded) {
            if (!xhr.addedTotal) {
              xhr.addedTotal = true;
              if (!Module.dataFileDownloads) Module.dataFileDownloads = {};
              Module.dataFileDownloads[url] = {
                loaded: event.loaded,
                total: size
              };
            } else {
              Module.dataFileDownloads[url].loaded = event.loaded;
            }
            var total = 0;
            var loaded = 0;
            var num = 0;
            for (var download in Module.dataFileDownloads) {
            var data = Module.dataFileDownloads[download];
              total += data.total;
              loaded += data.loaded;
              num++;
            }
            total = Math.ceil(total * Module.expectedDataFileDownloads/num);
            if (Module['setStatus']) Module['setStatus'](`Downloading data... (${loaded}/${total})`);
          } else if (!Module.dataFileDownloads) {
            if (Module['setStatus']) Module['setStatus']('Downloading data...');
          }
        };
        xhr.onerror = function(event) {
          throw new Error("NetworkError for: " + packageName);
        }
        xhr.onload = function(event) {
          if (xhr.status == 200 || xhr.status == 304 || xhr.status == 206 || (xhr.status == 0 && xhr.response)) { // file URLs can return 0
            var packageData = xhr.response;
            callback(packageData);
          } else {
            throw new Error(xhr.statusText + " : " + xhr.responseURL);
          }
        };
        xhr.send(null);
      };

      function handleError(error) {
        console.error('package error:', error);
      };

      var fetchedCallback = null;
      var fetched = Module['getPreloadedPackage'] ? Module['getPreloadedPackage'](REMOTE_PACKAGE_NAME, REMOTE_PACKAGE_SIZE) : null;

      if (!fetched) fetchRemotePackage(REMOTE_PACKAGE_NAME, REMOTE_PACKAGE_SIZE, function(data) {
        if (fetchedCallback) {
          fetchedCallback(data);
          fetchedCallback = null;
        } else {
          fetched = data;
        }
      }, handleError);

    function runWithFS() {

      function assert(check, msg) {
        if (!check) throw msg + new Error().stack;
      }
Module['FS_createPath']("/", "examples", true, true);
Module['FS_createPath']("/examples", "gb_spu", true, true);
Module['FS_createPath']("/examples/gb_spu", "metron", true, true);
Module['FS_createPath']("/examples", "ibex", true, true);
Module['FS_createPath']("/examples/ibex", "metron", true, true);
Module['FS_createPath']("/examples", "j1", true, true);
Module['FS_createPath']("/examples/j1", "metron", true, true);
Module['FS_createPath']("/examples", "pong", true, true);
Module['FS_createPath']("/examples/pong", "metron", true, true);
Module['FS_createPath']("/examples/pong", "reference", true, true);
Module['FS_createPath']("/examples", "rvsimple", true, true);
Module['FS_createPath']("/examples/rvsimple", "metron", true, true);
Module['FS_createPath']("/examples", "tutorial", true, true);
Module['FS_createPath']("/examples", "uart", true, true);
Module['FS_createPath']("/examples/uart", "metron", true, true);
Module['FS_createPath']("/", "tests", true, true);
Module['FS_createPath']("/tests", "metron_bad", true, true);
Module['FS_createPath']("/tests", "metron_good", true, true);

      /** @constructor */
      function DataRequest(start, end, audio) {
        this.start = start;
        this.end = end;
        this.audio = audio;
      }
      DataRequest.prototype = {
        requests: {},
        open: function(mode, name) {
          this.name = name;
          this.requests[name] = this;
          Module['addRunDependency'](`fp ${this.name}`);
        },
        send: function() {},
        onload: function() {
          var byteArray = this.byteArray.subarray(this.start, this.end);
          this.finish(byteArray);
        },
        finish: function(byteArray) {
          var that = this;
          // canOwn this data in the filesystem, it is a slide into the heap that will never change
          Module['FS_createDataFile'](this.name, null, byteArray, true, true, true);
          Module['removeRunDependency'](`fp ${that.name}`);
          this.requests[this.name] = null;
        }
      };

      var files = metadata['files'];
      for (var i = 0; i < files.length; ++i) {
        new DataRequest(files[i]['start'], files[i]['end'], files[i]['audio'] || 0).open('GET', files[i]['filename']);
      }

      function processPackageData(arrayBuffer) {
        assert(arrayBuffer, 'Loading data file failed.');
        assert(arrayBuffer.constructor.name === ArrayBuffer.name, 'bad input to processPackageData');
        var byteArray = new Uint8Array(arrayBuffer);
        var curr;
        // Reuse the bytearray from the XHR as the source for file reads.
          DataRequest.prototype.byteArray = byteArray;
          var files = metadata['files'];
          for (var i = 0; i < files.length; ++i) {
            DataRequest.prototype.requests[files[i].filename].onload();
          }          Module['removeRunDependency']('datafile_docs/demo/examples.data');

      };
      Module['addRunDependency']('datafile_docs/demo/examples.data');

      if (!Module.preloadResults) Module.preloadResults = {};

      Module.preloadResults[PACKAGE_NAME] = {fromCache: false};
      if (fetched) {
        processPackageData(fetched);
        fetched = null;
      } else {
        fetchedCallback = processPackageData;
      }

    }
    if (Module['calledRun']) {
      runWithFS();
    } else {
      if (!Module['preRun']) Module['preRun'] = [];
      Module["preRun"].push(runWithFS); // FS is not initialized yet, wait for it
    }

    }
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22817}, {"filename": "/examples/ibex/README.md", "start": 22817, "end": 22868}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22868, "end": 24466}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24466, "end": 36565}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36565, "end": 50989}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 50989, "end": 67025}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 67025, "end": 69539}, {"filename": "/examples/j1/metron/dpram.h", "start": 69539, "end": 69990}, {"filename": "/examples/j1/metron/j1.h", "start": 69990, "end": 74023}, {"filename": "/examples/pong/README.md", "start": 74023, "end": 74083}, {"filename": "/examples/pong/metron/pong.h", "start": 74083, "end": 77960}, {"filename": "/examples/pong/reference/README.md", "start": 77960, "end": 78020}, {"filename": "/examples/rvsimple/README.md", "start": 78020, "end": 78099}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78099, "end": 78612}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 78612, "end": 80086}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 80086, "end": 82704}, {"filename": "/examples/rvsimple/metron/config.h", "start": 82704, "end": 83932}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 83932, "end": 89664}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 89664, "end": 90787}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 90787, "end": 92733}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 92733, "end": 93973}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 93973, "end": 95230}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 95230, "end": 95912}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 95912, "end": 96889}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 96889, "end": 99020}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 99020, "end": 99797}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 99797, "end": 100490}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100490, "end": 101319}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 101319, "end": 102331}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 102331, "end": 103315}, {"filename": "/examples/rvsimple/metron/register.h", "start": 103315, "end": 104013}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 104013, "end": 107069}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 107069, "end": 112656}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 112656, "end": 115162}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 115162, "end": 119902}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 119902, "end": 121903}, {"filename": "/examples/scratch.h", "start": 121903, "end": 122420}, {"filename": "/examples/tutorial/adder.h", "start": 122420, "end": 122600}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 122600, "end": 123594}, {"filename": "/examples/tutorial/blockram.h", "start": 123594, "end": 124124}, {"filename": "/examples/tutorial/checksum.h", "start": 124124, "end": 124860}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 124860, "end": 125396}, {"filename": "/examples/tutorial/counter.h", "start": 125396, "end": 125545}, {"filename": "/examples/tutorial/declaration_order.h", "start": 125545, "end": 126324}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 126324, "end": 127742}, {"filename": "/examples/tutorial/nonblocking.h", "start": 127742, "end": 127864}, {"filename": "/examples/tutorial/submodules.h", "start": 127864, "end": 128981}, {"filename": "/examples/tutorial/templates.h", "start": 128981, "end": 129470}, {"filename": "/examples/tutorial/tutorial2.h", "start": 129470, "end": 129538}, {"filename": "/examples/tutorial/tutorial3.h", "start": 129538, "end": 129579}, {"filename": "/examples/tutorial/tutorial4.h", "start": 129579, "end": 129620}, {"filename": "/examples/tutorial/tutorial5.h", "start": 129620, "end": 129661}, {"filename": "/examples/tutorial/vga.h", "start": 129661, "end": 130821}, {"filename": "/examples/uart/README.md", "start": 130821, "end": 131065}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 131065, "end": 133658}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 133658, "end": 136232}, {"filename": "/examples/uart/metron/uart_top.h", "start": 136232, "end": 138009}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 138009, "end": 141041}, {"filename": "/tests/metron_bad/README.md", "start": 141041, "end": 141238}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 141238, "end": 141547}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 141547, "end": 141808}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 141808, "end": 142064}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 142064, "end": 142672}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 142672, "end": 142912}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 142912, "end": 143335}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 143335, "end": 143888}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 143888, "end": 144212}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 144212, "end": 144671}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 144671, "end": 144944}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 144944, "end": 145229}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 145229, "end": 145749}, {"filename": "/tests/metron_good/README.md", "start": 145749, "end": 145830}, {"filename": "/tests/metron_good/all_func_types.h", "start": 145830, "end": 147502}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 147502, "end": 147922}, {"filename": "/tests/metron_good/basic_function.h", "start": 147922, "end": 148214}, {"filename": "/tests/metron_good/basic_increment.h", "start": 148214, "end": 148582}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 148582, "end": 148890}, {"filename": "/tests/metron_good/basic_literals.h", "start": 148890, "end": 149515}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 149515, "end": 149774}, {"filename": "/tests/metron_good/basic_output.h", "start": 149774, "end": 150048}, {"filename": "/tests/metron_good/basic_param.h", "start": 150048, "end": 150320}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 150320, "end": 150564}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 150564, "end": 150757}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 150757, "end": 151033}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 151033, "end": 151266}, {"filename": "/tests/metron_good/basic_submod.h", "start": 151266, "end": 151568}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 151568, "end": 151936}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 151936, "end": 152325}, {"filename": "/tests/metron_good/basic_switch.h", "start": 152325, "end": 152815}, {"filename": "/tests/metron_good/basic_task.h", "start": 152815, "end": 153162}, {"filename": "/tests/metron_good/basic_template.h", "start": 153162, "end": 153661}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 153661, "end": 153833}, {"filename": "/tests/metron_good/bit_casts.h", "start": 153833, "end": 159819}, {"filename": "/tests/metron_good/bit_concat.h", "start": 159819, "end": 160259}, {"filename": "/tests/metron_good/bit_dup.h", "start": 160259, "end": 160931}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 160931, "end": 161782}, {"filename": "/tests/metron_good/builtins.h", "start": 161782, "end": 162126}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 162126, "end": 162446}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 162446, "end": 163013}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 163013, "end": 163853}, {"filename": "/tests/metron_good/constructor_args.h", "start": 163853, "end": 164325}, {"filename": "/tests/metron_good/defines.h", "start": 164325, "end": 164652}, {"filename": "/tests/metron_good/dontcare.h", "start": 164652, "end": 164945}, {"filename": "/tests/metron_good/enum_simple.h", "start": 164945, "end": 166387}, {"filename": "/tests/metron_good/for_loops.h", "start": 166387, "end": 166720}, {"filename": "/tests/metron_good/good_order.h", "start": 166720, "end": 167029}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 167029, "end": 167453}, {"filename": "/tests/metron_good/include_guards.h", "start": 167453, "end": 167650}, {"filename": "/tests/metron_good/init_chain.h", "start": 167650, "end": 168371}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 168371, "end": 168671}, {"filename": "/tests/metron_good/input_signals.h", "start": 168671, "end": 169346}, {"filename": "/tests/metron_good/local_localparam.h", "start": 169346, "end": 169537}, {"filename": "/tests/metron_good/magic_comments.h", "start": 169537, "end": 169853}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 169853, "end": 170180}, {"filename": "/tests/metron_good/minimal.h", "start": 170180, "end": 170268}, {"filename": "/tests/metron_good/multi_tick.h", "start": 170268, "end": 170647}, {"filename": "/tests/metron_good/namespaces.h", "start": 170647, "end": 171010}, {"filename": "/tests/metron_good/nested_structs.h", "start": 171010, "end": 171438}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 171438, "end": 171996}, {"filename": "/tests/metron_good/oneliners.h", "start": 171996, "end": 172272}, {"filename": "/tests/metron_good/plus_equals.h", "start": 172272, "end": 172696}, {"filename": "/tests/metron_good/private_getter.h", "start": 172696, "end": 172933}, {"filename": "/tests/metron_good/structs.h", "start": 172933, "end": 173165}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 173165, "end": 173714}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 173714, "end": 176271}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 176271, "end": 177038}, {"filename": "/tests/metron_good/tock_task.h", "start": 177038, "end": 177407}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 177407, "end": 177585}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 177585, "end": 178257}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 178257, "end": 178929}], "remote_package_size": 178929});

  })();
