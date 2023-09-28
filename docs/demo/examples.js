
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22885}, {"filename": "/examples/ibex/README.md", "start": 22885, "end": 22936}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22936, "end": 24581}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24581, "end": 36772}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36772, "end": 51270}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 51270, "end": 67353}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 67353, "end": 69944}, {"filename": "/examples/j1/metron/dpram.h", "start": 69944, "end": 70395}, {"filename": "/examples/j1/metron/j1.h", "start": 70395, "end": 74428}, {"filename": "/examples/pong/README.md", "start": 74428, "end": 74488}, {"filename": "/examples/pong/metron/pong.h", "start": 74488, "end": 78390}, {"filename": "/examples/pong/reference/README.md", "start": 78390, "end": 78450}, {"filename": "/examples/rvsimple/README.md", "start": 78450, "end": 78529}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78529, "end": 79042}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 79042, "end": 80516}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 80516, "end": 83134}, {"filename": "/examples/rvsimple/metron/config.h", "start": 83134, "end": 84362}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 84362, "end": 90094}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 90094, "end": 91217}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 91217, "end": 93163}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 93163, "end": 94403}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 94403, "end": 95660}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 95660, "end": 96342}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 96342, "end": 97319}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 97319, "end": 99450}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 99450, "end": 100227}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 100227, "end": 100920}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100920, "end": 101749}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 101749, "end": 102761}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 102761, "end": 103745}, {"filename": "/examples/rvsimple/metron/register.h", "start": 103745, "end": 104443}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 104443, "end": 107499}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 107499, "end": 113086}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 113086, "end": 115592}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 115592, "end": 120332}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 120332, "end": 122333}, {"filename": "/examples/scratch.h", "start": 122333, "end": 122614}, {"filename": "/examples/tutorial/adder.h", "start": 122614, "end": 122794}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 122794, "end": 123788}, {"filename": "/examples/tutorial/blockram.h", "start": 123788, "end": 124318}, {"filename": "/examples/tutorial/checksum.h", "start": 124318, "end": 125054}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 125054, "end": 125590}, {"filename": "/examples/tutorial/counter.h", "start": 125590, "end": 125739}, {"filename": "/examples/tutorial/declaration_order.h", "start": 125739, "end": 126518}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 126518, "end": 127936}, {"filename": "/examples/tutorial/nonblocking.h", "start": 127936, "end": 128058}, {"filename": "/examples/tutorial/submodules.h", "start": 128058, "end": 129175}, {"filename": "/examples/tutorial/templates.h", "start": 129175, "end": 129664}, {"filename": "/examples/tutorial/tutorial2.h", "start": 129664, "end": 129732}, {"filename": "/examples/tutorial/tutorial3.h", "start": 129732, "end": 129773}, {"filename": "/examples/tutorial/tutorial4.h", "start": 129773, "end": 129814}, {"filename": "/examples/tutorial/tutorial5.h", "start": 129814, "end": 129855}, {"filename": "/examples/tutorial/vga.h", "start": 129855, "end": 131015}, {"filename": "/examples/uart/README.md", "start": 131015, "end": 131259}, {"filename": "/examples/uart/message.txt", "start": 131259, "end": 131771}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 131771, "end": 134364}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 134364, "end": 136938}, {"filename": "/examples/uart/metron/uart_top.h", "start": 136938, "end": 138715}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 138715, "end": 141747}, {"filename": "/tests/metron_bad/README.md", "start": 141747, "end": 141944}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 141944, "end": 142253}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 142253, "end": 142514}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 142514, "end": 142770}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 142770, "end": 143378}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 143378, "end": 143629}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 143629, "end": 144052}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 144052, "end": 144605}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 144605, "end": 144929}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 144929, "end": 145388}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 145388, "end": 145651}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 145651, "end": 145936}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 145936, "end": 146456}, {"filename": "/tests/metron_bad/wrong_submod_call_order.h", "start": 146456, "end": 146840}, {"filename": "/tests/metron_good/README.md", "start": 146840, "end": 146921}, {"filename": "/tests/metron_good/all_func_types.h", "start": 146921, "end": 148599}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 148599, "end": 149019}, {"filename": "/tests/metron_good/basic_function.h", "start": 149019, "end": 149311}, {"filename": "/tests/metron_good/basic_increment.h", "start": 149311, "end": 149679}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 149679, "end": 149987}, {"filename": "/tests/metron_good/basic_literals.h", "start": 149987, "end": 150612}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 150612, "end": 150871}, {"filename": "/tests/metron_good/basic_output.h", "start": 150871, "end": 151145}, {"filename": "/tests/metron_good/basic_param.h", "start": 151145, "end": 151417}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 151417, "end": 151661}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 151661, "end": 151854}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 151854, "end": 152130}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 152130, "end": 152363}, {"filename": "/tests/metron_good/basic_submod.h", "start": 152363, "end": 152682}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 152682, "end": 153050}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 153050, "end": 153439}, {"filename": "/tests/metron_good/basic_switch.h", "start": 153439, "end": 153929}, {"filename": "/tests/metron_good/basic_task.h", "start": 153929, "end": 154276}, {"filename": "/tests/metron_good/basic_template.h", "start": 154276, "end": 154775}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 154775, "end": 154947}, {"filename": "/tests/metron_good/bit_casts.h", "start": 154947, "end": 160933}, {"filename": "/tests/metron_good/bit_concat.h", "start": 160933, "end": 161373}, {"filename": "/tests/metron_good/bit_dup.h", "start": 161373, "end": 162045}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 162045, "end": 162953}, {"filename": "/tests/metron_good/builtins.h", "start": 162953, "end": 163297}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 163297, "end": 163617}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 163617, "end": 164184}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 164184, "end": 165038}, {"filename": "/tests/metron_good/constructor_args.h", "start": 165038, "end": 165510}, {"filename": "/tests/metron_good/defines.h", "start": 165510, "end": 165837}, {"filename": "/tests/metron_good/dontcare.h", "start": 165837, "end": 166130}, {"filename": "/tests/metron_good/enum_simple.h", "start": 166130, "end": 167572}, {"filename": "/tests/metron_good/for_loops.h", "start": 167572, "end": 167905}, {"filename": "/tests/metron_good/good_order.h", "start": 167905, "end": 168214}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 168214, "end": 168638}, {"filename": "/tests/metron_good/include_guards.h", "start": 168638, "end": 168835}, {"filename": "/tests/metron_good/init_chain.h", "start": 168835, "end": 169556}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 169556, "end": 169854}, {"filename": "/tests/metron_good/input_signals.h", "start": 169854, "end": 170652}, {"filename": "/tests/metron_good/local_localparam.h", "start": 170652, "end": 170843}, {"filename": "/tests/metron_good/magic_comments.h", "start": 170843, "end": 171159}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 171159, "end": 171486}, {"filename": "/tests/metron_good/minimal.h", "start": 171486, "end": 171672}, {"filename": "/tests/metron_good/multi_tick.h", "start": 171672, "end": 172051}, {"filename": "/tests/metron_good/namespaces.h", "start": 172051, "end": 172414}, {"filename": "/tests/metron_good/nested_structs.h", "start": 172414, "end": 172874}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 172874, "end": 173432}, {"filename": "/tests/metron_good/oneliners.h", "start": 173432, "end": 173708}, {"filename": "/tests/metron_good/plus_equals.h", "start": 173708, "end": 174132}, {"filename": "/tests/metron_good/private_getter.h", "start": 174132, "end": 174369}, {"filename": "/tests/metron_good/structs.h", "start": 174369, "end": 174601}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 174601, "end": 175150}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 175150, "end": 177707}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 177707, "end": 178474}, {"filename": "/tests/metron_good/tock_task.h", "start": 178474, "end": 178855}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 178855, "end": 179033}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 179033, "end": 179705}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 179705, "end": 180377}], "remote_package_size": 180377});

  })();
