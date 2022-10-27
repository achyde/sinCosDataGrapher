<template>
  <div>
    <div ref="plot"></div>
  </div>
</template>

<script setup lang="ts">
import {onMounted, ref, defineProps} from 'vue'
import {watchThrottled} from "@vueuse/core"
import Plotly from 'plotly.js-dist-min'

const plot = ref(null);
interface DataGraphProps {
  data: Array<Array<FloatTimeEntry>>
  labels?: Array<string>
  title?: string
}
const props = defineProps<DataGraphProps>();

const dataHasChanged = (data: Array<Array<FloatTimeEntry>>) => {

  const dataToGraph: Array<XYEntry> = [];
  data.forEach((arrayOfEntries, index) => {

    dataToGraph.push({x: arrayOfEntries.map(item => item.time), y: arrayOfEntries.map(item => item.val)});

    if (props.labels && props.labels[index] !== undefined) {
      dataToGraph[dataToGraph.length - 1].showlegend = true;
      dataToGraph[dataToGraph.length - 1].name = props.labels[index];
    }
  })

  Plotly.react(plot.value!, dataToGraph, {title: props.title});
}

onMounted(() => {

  setTimeout(()=>{

    dataHasChanged(props.data);

    watchThrottled(() => props.data, () => dataHasChanged(props.data),
        {
          deep: true,
          throttle: 200
        });

  }, 100);
});

</script>

