// Composables
import { createRouter, createWebHistory } from 'vue-router'

const routes = [
  {
    path: '/',
    name: 'Home',
    component: () => import('@/views/Intro.vue') // Redirects base URL to Intro
  },
  {
    path: '/intro',
    name: 'Intro',
    component: () => import('@/views/Intro.vue')
  },
  {
    path: '/analysis',
    name: 'Analysis',
    component: () => import('@/views/Analysis.vue') // Ensure Analysis.vue exists
  },
  {
    path: '/graph',
    name: 'Graph',
    component: () => import('@/views/Graph.vue')
  },
]

const router = createRouter({
  history: createWebHistory(process.env.BASE_URL),
  routes,
})

export default router