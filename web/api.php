<?php
$accion = $_GET['accion'];

$host = "localhost";
$user = "root";
$password = "";
$dbname = "arduino_db";

$conn = new mysqli($host, $user, $password, $dbname);

if ($conn->connect_error) {
    die("Conexión fallida: " . $conn->connect_error);
}

if ($accion == 'listar') {
    $sql = "SELECT * FROM tarjetas";
    $result = $conn->query($sql);
    $tarjetas = [];

    while($row = $result->fetch_assoc()) {
        $tarjetas[] = $row;
    }

    echo json_encode($tarjetas);
} elseif ($accion == 'activar') {
    $id = $_GET['id'];
    $sql = "UPDATE tarjetas SET activado = 1 WHERE id = $id";

    if ($conn->query($sql) === TRUE) {
        echo json_encode(["success" => true]);
    } else {
        echo json_encode(["success" => false, "error" => $conn->error]);
    }
} elseif ($accion == 'desactivar') {
    $id = $_GET['id'];
    $sql = "UPDATE tarjetas SET activado = 0 WHERE id = $id";

    if ($conn->query($sql) === TRUE) {
        echo json_encode(["success" => true]);
    } else {
        echo json_encode(["success" => false, "error" => $conn->error]);
    }
} elseif ($accion == 'eliminar') {
    $id = $_GET['id'];
    $sql = "DELETE FROM tarjetas WHERE id = $id";

    if ($conn->query($sql) === TRUE) {
        echo json_encode(["success" => true]);
    } else {
        echo json_encode(["success" => false, "error" => $conn->error]);
    }
} elseif ($accion == 'alta') {
    $data = json_decode(file_get_contents('php://input'), true);
    $codigo = $data['codigo'];

    // Inserta la respuesta de la tarjeta directamente en la tabla tarjetas_respuesta
    $sql = "INSERT INTO tarjetas (codigo) VALUES ('$codigo')";  // Por defecto se guarda como no aceptado (0)

    if ($conn->query($sql) === TRUE) {
        echo json_encode(["success" => true]);
    } else {
        echo json_encode(["success" => false, "error" => $conn->error]);
    }
} elseif ($accion == 'listar_respuestas') {
    $sql = "SELECT * FROM tarjetas_respuesta";
    $result = $conn->query($sql);
    $respuestas = [];

    while($row = $result->fetch_assoc()) {
        $respuestas[] = $row;
    }

    echo json_encode($respuestas);
}   

$conn->close();
?>
