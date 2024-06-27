# Motion-Simulator-Client-Server

## Physics Principles

Projectile motion refers to the motion of an object thrown into the air at an angle. The object is influenced by gravity and air resistance (which we are ignoring for simplicity). The key physics equations governing projectile motion are:

  ### Horizontal motion:
        Velocity (v_x) = v_0 * cos(θ)
        Displacement (x) = v_x * t

  ### Vertical motion:
        Velocity (v_y) = v_0 * sin(θ) - g * t
        Displacement (y) = v_0 * sin(θ) * t - 0.5 * g * t²

  ### Key equations derived from the above:
        Time of flight (T) = 2 * v_0 * sin(θ) / g
        Maximum height (H) = (v_0² * sin²(θ)) / (2 * g)
        Range (R) = (v_0² * sin(2 * θ)) / g

Where:

    v0v0​ is the initial velocity
    θ is the launch angle
    g is the acceleration due to gravity (9.81 m/s²)

## Data Structures

We have defined three main data structures to store projectile motion parameters and results:

### ProjectileParams:

typedef struct {
    double initial_velocity;
    double launch_angle;
} ProjectileParams;

    initial_velocity: The initial speed of the projectile.
    launch_angle: The angle at which the projectile is launched.

### ProjectileResults:

typedef struct {
    double max_height;
    double time_of_flight;
    double range;
} ProjectileResults;

    max_height: The maximum height reached by the projectile.
    time_of_flight: The total time the projectile is in the air.
    range: The horizontal distance traveled by the projectile.

### SimulationData:

    typedef struct {
        ProjectileParams params;
        ProjectileResults results;
    } SimulationData;

        params: The initial conditions of the projectile.
        results: The calculated results based on the initial conditions.

## Implementation Details

Server Implementation

  ### Server Setup:
        The server listens on a specific port (8091).
        It handles incoming client connections and processes their requests.

  ### Request Handling:
        GET Requests:
            Clients send initial velocity and launch angle.
            The server calculates the trajectory using the physics equations.
            The results (max height, time of flight, range) are sent back to the client.
        POST Requests:
            Clients can store pre-recorded simulation data.
            The server stores this data if there's enough space.
        Retrieve Requests:
            Clients can request stored simulation data.
            The server sends all stored data back to the client.

## Client Implementation

  ### Client Setup:
        The client connects to the server on the specified port.
        It sends GET and POST requests based on user input.

  ### User Input Handling:
        Users input initial velocity and launch angle.
        The client sends this data as a GET request to the server.
        The server's response is displayed to the user.
        Users can choose to post the simulation data to the server.

  ### Error Handling:
        The client handles invalid input parameters (e.g., negative velocity, out-of-range angles).
        The server responds with appropriate error messages if requests are malformed or if storage limits are reached.

## Error Handling

    Invalid Input Parameters:
        The client ensures that the initial velocity is non-negative and the launch angle is between 0 and 180 degrees.
    Network Issues:
        Both client and server handle socket errors and connection issues gracefully.
    Storage Limit:
        The server checks if there's enough space before storing new simulation data and responds with an error message if the limit is reached.

## Compiling the Server
      gcc server.c -o server -lm
## Compiling the Client
      gcc client.c -o client -lm


