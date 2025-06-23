<script setup>
import image from './assets/img/TrashBin.png'
import { onMounted, onBeforeUnmount, reactive, computed } from 'vue'
import mqtt from 'mqtt'

// Reactive data for MQTT states
const state = reactive({
  ledStatus: 'Mati',
  trashStatus: 'AMAN',
  distance: '0 cm',
  servoStatus: 'Nonaktif',
  cautionMessage: 'Tempat sampah belum penuh',
  showProfile: false
})

let client = null

function toggleProfile() {
  state.showProfile = !state.showProfile
}

// Dynamic class based on status
const ledClass = computed(() => state.ledStatus === 'Hidup' ? 'text-green-300' : 'text-white')
const servoClass = computed(() => state.servoStatus === 'Aktif' ? 'text-green-300' : 'text-white')
const trashClass = computed(() => state.trashStatus === 'PENUH' ? 'text-red-500' : 'text-green-300')
const cautionClass = computed(() => state.trashStatus === 'PENUH' ? 'bg-red-100' : 'bg-green-50')
const cautionBarClass = computed(() => state.trashStatus === 'PENUH' ? 'bg-red-700' : 'bg-green-700')

onMounted(() => {
  const options = {
    clientId: 'web_client_' + Math.random().toString(16).substr(2, 8),
    keepalive: 60,
    clean: true
  }

  client = mqtt.connect("wss://broker.hivemq.com:8884/mqtt", options)

  client.on('connect', () => {
    console.log('Connected to MQTT broker')
    client.subscribe('iot/trash/led')
    client.subscribe('iot/trash/status')
    client.subscribe('iot/trash/distance')
    client.subscribe('iot/trash/servo')
  })

  client.on('message', (topic, message) => {
    const msg = message.toString();

    if (topic === 'iot/trash/led') {
      state.ledStatus = msg === 'ON' ? 'Hidup' : 'Mati';
    } else if (topic === 'iot/trash/status') {
      state.trashStatus = msg === 'PENUH' ? 'PENUH' : 'AMAN';
      state.cautionMessage = msg === 'PENUH'
        ? 'Tempat sampah sudah penuh, segera kosongkan!'
        : 'Tempat sampah belum penuh';
    } else if (topic === 'iot/trash/distance') {
      state.distance = msg + ' cm';
    } else if (topic === 'iot/trash/servo') {
      state.servoStatus = msg === 'AKTIF' ? 'Aktif' : 'Nonaktif';
    }

    console.log(`Topic: ${topic}, Message: ${msg}`);
  })
})

onBeforeUnmount(() => {
  if (client) {
    client.end()
  }
})
</script>

<template>
  <section class="flex justify-center items-center min-h-screen w-full relative font-poppins bg-gradient-to-t from-amber-500/60 to-white p-4">
    <div class="w-full max-w-[900px] lg:w-[900px] h-auto lg:h-[450px] bg-white rounded-3xl relative p-4 lg:p-6 shadow-2xl flex flex-col justify-center">
      <div class="mb-4">
        <h1 class="text-2xl lg:text-4xl font-bold text-red-900">Dashboard</h1>
        <i class="text-xs lg:text-sm">IoT - Tempat Sampah Otomatis</i>
      </div>

      <!-- Status Cards -->
      <div class="w-full max-w-[560px] flex flex-wrap justify-between gap-2 lg:gap-3 text-white mb-4 lg:mb-0">
        <div class="bg-red-900 h-28 lg:h-36 w-[calc(50%-4px)] sm:w-28 lg:w-32 rounded-xl shadow-md flex justify-center items-center flex-col">
          <p :class="['text-lg lg:text-2xl font-bold', ledClass]">{{ state.ledStatus }}</p>
          <h1 class="text-xs">Status LED</h1>
        </div>
        <div class="bg-red-900 h-28 lg:h-36 w-[calc(50%-4px)] sm:w-28 lg:w-32 rounded-xl shadow-md flex justify-center items-center flex-col">
          <p :class="['text-lg lg:text-2xl font-bold', trashClass]">{{ state.trashStatus }}</p>
          <h1 class="text-xs">Status Sampah</h1>
        </div>
        <div class="bg-red-900 h-28 lg:h-36 w-[calc(50%-4px)] sm:w-28 lg:w-32 rounded-xl shadow-md flex justify-center items-center flex-col">
          <p class="text-lg lg:text-2xl font-bold text-white">{{ state.distance }}</p>
          <h1 class="text-xs">Jarak Terdeteksi</h1>
        </div>
        <div class="bg-red-900 h-28 lg:h-36 w-[calc(50%-4px)] sm:w-28 lg:w-32 rounded-xl shadow-md flex justify-center items-center flex-col">
          <p :class="['text-lg lg:text-2xl font-bold', servoClass]">{{ state.servoStatus }}</p>
          <h1 class="text-xs">Servo Motor</h1>
        </div>
      </div>

      <!-- Caution Box -->
      <div :class="['w-full max-w-[560px] h-12 lg:h-14 my-3 lg:my-5 rounded-xl flex items-center gap-4 lg:gap-6', cautionClass]">
        <div :class="['w-4 lg:w-5 h-full rounded-l-xl', cautionBarClass]"></div>
        <h1 class="font-medium text-sm lg:text-base text-black">{{ state.cautionMessage }}</h1>
      </div>
    </div>

    <!-- Profile Panel -->
    <div class="fixed lg:absolute z-50 bg-red-900 w-full lg:w-72 h-screen right-0 lg:right-44 flex flex-col justify-center items-center text-white p-4 lg:p-5 transform transition-transform duration-300"
      :class="state.showProfile ? 'translate-x-0' : 'translate-x-full lg:translate-x-0'">
      <button @click="toggleProfile" class="absolute top-4 left-4 lg:hidden text-white text-2xl z-10">&times;</button>
      <h1 class="text-2xl lg:text-4xl font-bold text-white text-center">Smart Trash</h1>
      <img :src="image" alt="Trash bin" class="w-32 lg:w-44 my-6 lg:my-10">
      <div class="w-full">
        <h1 class="font-bold text-center lg:text-left">Anggota Tim</h1>
        <div class="flex flex-col gap-2 lg:gap-3 my-2 items-center sm:items-stretch">
          <div class="bg-white w-64 sm:w-full h-10 lg:h-12 rounded-2xl flex gap-3 lg:gap-5 text-zinc-900 items-center px-2">
            <img src="/img/cagita.jpeg" alt="" class="w-8 h-8 lg:w-9 lg:h-9 rounded-full">
            <div class="min-w-0 flex-1">
              <p class="font-medium text-xs truncate">Cagita Dian A'yunin</p>
              <p class="text-xs lg:text-sm">233140707111025</p>
            </div>
          </div>
          <div class="bg-white w-64 sm:w-full h-10 lg:h-12 rounded-2xl flex gap-3 lg:gap-5 text-zinc-900 items-center px-2">
            <img src="/img/rangga.jpeg" alt="" class="w-8 h-8 lg:w-9 lg:h-9 rounded-full">
            <div class="min-w-0 flex-1">
              <p class="font-medium text-xs truncate">TB Rangga Gilang Y.</p>
              <p class="text-xs lg:text-sm">233140707111032</p>
            </div>
          </div>
          <div class="bg-white w-64 sm:w-full h-10 lg:h-12 rounded-2xl flex gap-3 lg:gap-5 text-zinc-900 items-center px-2">
            <img src="/img/devha.jpeg" alt="" class="w-8 h-8 lg:w-9 lg:h-9 rounded-full">
            <div class="min-w-0 flex-1">
              <p class="font-medium text-xs truncate">Fredlina Devhania K.</p>
              <p class="text-xs lg:text-sm">233140707111035</p>
            </div>
          </div>
        </div>
      </div>
    </div>

    <!-- Mobile menu button -->
    <button @click="toggleProfile" class="fixed top-4 right-4 z-60 lg:hidden bg-red-900 text-white p-3 rounded-full shadow-lg">
      <svg class="w-6 h-6" fill="none" stroke="currentColor" viewBox="0 0 24 24">
        <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M4 6h16M4 12h16M4 18h16"></path>
      </svg>
    </button>

    <!-- Overlay -->
    <div v-if="state.showProfile" @click="toggleProfile" class="fixed inset-0 bg-black bg-opacity-50 z-40 lg:hidden"></div>
  </section>
</template>
