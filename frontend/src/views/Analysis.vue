<template>
  <v-container fluid>
    <v-row>
      <v-col cols="12" md="6" align="center">
        <v-card class="pa-5" elevation="2">
          <v-card-title>Frequency of Random Numbers</v-card-title>
          <canvas id="myChart"></canvas>
        </v-card>
      </v-col>
      <v-col cols="12" md="6" align="center">
        <v-btn 
          text="Refresh Graph" 
          color="secondary" 
          variant="outlined" 
          flat 
          @click="updateGraph()">
        </v-btn>
      </v-col>
    </v-row>
    <v-row>
      <v-col cols="12" align="center">
        <div class="d-flex justify-center">
          <VCard class="ma-2" subtitle="LED A" width="150" flat border align="center">
            <VCardItem class="pa-0">Turned on</VCardItem>
            <VCardItem class="pa-0">
              <span class="text-h5 text-primary font-weight-bold">{{ led_A }}</span>
            </VCardItem>
            <VCardItem class="pa-0">times</VCardItem>
            <VCardItem>
              <VBtn 
                text="Update" 
                class="ma-1 text-caption" 
                rounded="pill" 
                flat 
                color="secondary" 
                variant="tonal" 
                @click="updateLEDCount('ledA')">
              </VBtn>
            </VCardItem>
          </VCard>

          <VCard class="ma-2" subtitle="LED B" width="150" flat border align="center">
            <VCardItem class="pa-0">Turned on</VCardItem>
            <VCardItem class="pa-0">
              <span class="text-h5 text-primary font-weight-bold">{{ led_B }}</span>
            </VCardItem>
            <VCardItem class="pa-0">times</VCardItem>
            <VCardItem>
              <VBtn 
                text="Update" 
                class="ma-1 text-caption" 
                rounded="pill" 
                flat 
                color="secondary" 
                variant="tonal" 
                @click="updateLEDCount('ledB')">
              </VBtn>
            </VCardItem>
          </VCard>
        </div>
      </v-col>
    </v-row>
  </v-container>
</template>

<script setup>
import { useAppStore } from "@/store/appStore";
import Chart from 'chart.js/auto';
import { onMounted, ref } from 'vue';

// VARIABLES
const AppStore = useAppStore();
const led_A = ref(0);
const led_B = ref(0);
let chart = null;

const data = {
  labels: ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9'],
  datasets: [{
    label: 'Number Frequency',
    data: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    backgroundColor: '#4BD5E7',
    borderColor: '#1ECBE1',
    borderWidth: 2
  }]
};

const config = {
  type: 'bar',
  data: data,
  options: { responsive: true }
};

// FUNCTIONS
onMounted(() => {
  const ctx = document.querySelector('#myChart');
  chart = new Chart(ctx, config);
});

const updateData = (chart, label, newData) => {
  chart.data.labels = label;
  chart.data.datasets[0].data = newData;
  chart.update();
};

const updateGraph = async () => {
  let result = await AppStore.getFrequencies();
  if (result.length > 0) {
    let labels = result.map(obj => obj["number"]);
    let frequencies = result.map(obj => obj["frequency"]);
    updateData(chart, labels, frequencies);
  }
};

const updateLEDCount = async (name) => {
  let result = await AppStore.getOnCount(name);
  if (name == "ledA") led_A.value = result;
  if (name == "ledB") led_B.value = result;
};
</script>