<template>
  <header class="flex items-center justify-center p-2 sticky top-0 z-50 border rounded bg-green-300">
    <label class="ml-auto">Collect Data <input type="checkbox" v-model="collectData"></label>
    <button @click="clearData" class="ml-2 border border-black rounded px-2 py-1 bg-orange-400" type="button">Clear Data</button>
  </header>
  <DataGraph
      :data="[sin_data, cos_data]"
      :labels="['Sin','Cos']"
      title="Sin / Cos"
      class="border rounded background-white mx-5 my-2"/>
</template>

<script setup lang="ts">
import DataGraph from './components/DataGraph.vue'
import {ref, reactive} from "vue";
import {throttle} from "lodash-es"

const MAX_BUFFER_LENGTH = 1500;
const collectData = ref(true);
const sin_data = reactive([]);
const cos_data = reactive([]);

const resizeBufferIfNeeded = (buffer: Array<FloatTimeEntry>, length:number) => {
  if(buffer.length > length){
    buffer.splice( 0, buffer.length - length);
  }
};

const addValueToBuffer = (buffer: Array<FloatTimeEntry>, value:number, time:number) => {
  if(!isNaN(value)){
    buffer.splice( buffer.length,0, {time: time, val: value} );
    resizeBufferIfNeeded(buffer, MAX_BUFFER_LENGTH);
  }
};

const clearData = () => {
  sin_data.length = 0;
  cos_data.length = 0;
}

let lastDataValues:Float64Array;
const OFFSETS = new Map();
OFFSETS.set("I", 0);
OFFSETS.set("SIN", 1);
OFFSETS.set("COS", 2);

const handleData = throttle(() => {
  try{
    if( collectData.value && lastDataValues.length !== undefined && lastDataValues.length >= OFFSETS.size){
      addValueToBuffer(sin_data, lastDataValues[OFFSETS.get("SIN")],  lastDataValues[OFFSETS.get("I")]);
      addValueToBuffer(cos_data, lastDataValues[OFFSETS.get("COS")],  lastDataValues[OFFSETS.get("I")]);
    }
  }catch (e) {
    console.error(e)
  }
},200)

window.api.receive("udp_data", (values:Float64Array) =>{
  lastDataValues = values;
  handleData();
});
</script>

