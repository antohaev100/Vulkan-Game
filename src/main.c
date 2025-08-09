#include "vk_fun.h"
#include "glfw_fun.h"
#include <stdbool.h>

// Function prototypes
static void *rendering_thread(void *arg);
static void *input_thread(void *arg);
static void *physics_thread(void *arg);
static void *audio_thread(void *arg);
static void framebufferSizeCallback(GLFWwindow *window, int width, int height);
static void barrier_wait();
static void initBuffer(sharedBuffer *pBuffer);

// file scope Synchronization variables
static struct cthreads_mutex mutex;
static struct cthreads_cond cond;
static int ready = 0;
static int iteration = 0;
static bool running = true;
static const int threadCount = 4;

int main() {
    glfwInit();
    char windowTitle[] = "Vulkan Triangle";
	GLFWwindow *pWindow = createVulkanWindow(windowTitle);
    printf("Window created\n");
    initVulkan(pWindow);
    printf("Vulkan initialized\n");
    timer_lib_initialize();
    printf("Timer initialized\n");
    // Create and initialize the application state
    sharedBuffer *pBuffer = malloc(sizeof(sharedBuffer));
    initBuffer(pBuffer);
    printf("Buffer initialized\n");

    struct cthreads_thread renderThread, inputThread, physicsThread, audioThread;
    struct cthreads_args renderArgs = { rendering_thread, pBuffer};
    struct cthreads_args physicsArgs = { physics_thread, pBuffer};
    struct cthreads_args audioArgs = { audio_thread, NULL };

    // Initialize synchronization variables
    cthreads_mutex_init(&mutex, NULL);
    cthreads_cond_init(&cond, NULL);

    // Create threads
    printf("Creating threads\n");
    cthreads_thread_create(&renderThread, NULL, rendering_thread, pBuffer, &renderArgs);
    cthreads_thread_create(&physicsThread, NULL, physics_thread, pBuffer, &physicsArgs);
    cthreads_thread_create(&audioThread, NULL, audio_thread, NULL, &audioArgs);


    // Make the window's context current
    glfwMakeContextCurrent(pWindow);

    // Set the user pointer for the window
    glfwSetWindowUserPointer(pWindow, pBuffer);

        // Set the cursor mode to disabled
    glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the key callback function
    glfwSetKeyCallback(pWindow, key_callback);

    // Set the mouse callback function
    glfwSetCursorPosCallback(pWindow, mouse_callback);

    // Set the framebuffer size callback
    glfwSetFramebufferSizeCallback(pWindow, framebufferSizeCallback);

        // Initialize the timer library
    if (timer_lib_initialize() != 0) {
        printf("Failed to initialize timer library\n");
        return -1;
    }

    tick_t lastTime = timer_current();
    while (!glfwWindowShouldClose(pWindow)) {
        glfwPollEvents();
        tick_t currentTime = timer_current();
        pBuffer->dt = timer_ticks_to_seconds(timer_elapsed_ticks(lastTime));
        lastTime = currentTime;
        //if(iteration % 600 == 0){printf("main done!\n");}
        barrier_wait();
    }
    printf("Stopping threads\n");
    running = false;
    barrier_wait();
    // Wait for threads to finish
    cthreads_thread_join(renderThread, NULL);
    cthreads_thread_join(physicsThread, NULL);
    cthreads_thread_join(audioThread, NULL);

    printf("Exiting main thread\n");
    // Destroy synchronization variables
    cthreads_mutex_destroy(&mutex);
    cthreads_cond_destroy(&cond);
    free(pBuffer);
    printf("Destroying Vulkan\n");
    deleteVulkan();
    printf("Destroying window\n");
    deleteWindow(pWindow);
    printf("Terminating GLFW\n");
    glfwTerminate();
    return 0;
}

static void initBuffer(sharedBuffer *pBuffer){
    pBuffer->cameraPos[0]=0.0f;
    pBuffer->cameraPos[1]=0.0f;
    pBuffer->cameraPos[2]=0.25f;
    pBuffer->cameraFront[0]=1.0f;
    pBuffer->cameraFront[1]=0.0f;
    pBuffer->cameraFront[2]=0.0f;
    pBuffer->yaw = 0.0f;
    pBuffer->thirdPerson = false;
    for (int i = 0; i < 6; i++) {
        pBuffer->cameraMoveInput[i] = false;
    }
    pBuffer->fov = 70.0f;
    pBuffer->dt = 0.0f;
    pBuffer->playerModel = (obj3d){
        .pos = {0.0f, 0.0f, 0.25f},
        .dimension = {0.5f, 0.5f, 0.5f},
        .color = {0.2f, 0.2f, 0.8f},
        .rotation = {0.0f, 0.0f, 0.0f}
    };
    initVector(&pBuffer->cuboids,sizeof(obj3d),1,1);
    initVector(&pBuffer->ellipsoids,sizeof(obj3d),1,1);
    initVector(&pBuffer->ellipsoidCylinders,sizeof(obj3d),1,1);
    obj3d Ball = {
        .pos = {5.0f, 5.0f, 5.5f},
        .dimension = {0.5f, 1.0f, 0.5f},
        .color = {0.8f, 0.3f, 0.8f},
        .rotation = {0.0f, 0.0f, 0.0f}
    };
    vectorAdd(&pBuffer->ellipsoids, &Ball);
    obj3d cube = {
        .pos = {0.0f, 2.0f, 2.0f},
        .dimension = {1.0f, 1.0f, 1.0f},
        .color = {0.8f, 0.8f, 0.8f},
        .rotation = {0.0f, 0.0f, 0.0f}
    };
    vectorAdd(&pBuffer->cuboids, &cube);
    obj3d pillar = {
        .pos = {5.0f, 0.0f, 1.0f},
        .dimension = {0.5f, 0.5f, 2.0f},
        .color = {0.8f, 0.8f, 0.8f},
        .rotation = {0.0f, 0.0f, 0.0f}
    };
    vectorAdd(&pBuffer->ellipsoidCylinders, &pillar);
}

static void barrier_wait() {
    cthreads_mutex_lock(&mutex);
    //printf("Barrier wait, ready: %d, iteration: %d\n", ready, iteration);
    ready++;
    if (ready == threadCount) {
        ready = 0;
        //if(iteration % 600 == 0){printf("start\n");}
        iteration++;
        cthreads_cond_broadcast(&cond);
    } else {
        int current_iteration = iteration;
        while (current_iteration == iteration) {
            cthreads_cond_wait(&cond, &mutex);
        }
    }
    //printf("Barrier passed, ready: %d, iteration: %d\n", ready, iteration);
    cthreads_mutex_unlock(&mutex);
}

static void framebufferSizeCallback(GLFWwindow *window, int width, int height){
    recreateSwapChain(window, width, height);
}

static void *rendering_thread(void *arg) {
    sharedBuffer *pBuffer = (sharedBuffer *)arg;
    while(running) {
        presentImage(*pBuffer);
        //if(iteration % 600 == 0){printf("render done!\n");}
        barrier_wait();
    }
    return NULL;
}

static void *physics_thread(void *arg) {
    sharedBuffer *pBuffer = (sharedBuffer *)arg;
    while (running) {
        update(pBuffer);
        //if(iteration % 600 == 0){printf("physics done!\n");}
        barrier_wait();
    }
    return NULL;
}

static void *audio_thread(void *arg) {
    while (running) {
        //audio();
        barrier_wait();
    }
    return NULL;
}