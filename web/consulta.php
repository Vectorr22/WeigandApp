<?php
$codigo = isset($_GET['codigo']) ? $_GET['codigo'] : '';
$host = "localhost";
$user = "root";
$password = "";
$dbname = "arduino_db";

$conn = new mysqli($host, $user, $password, $dbname);

if ($conn->connect_error) {
    die("Conexión fallida: " . $conn->connect_error);
}

// Verificar si el código de la tarjeta existe y está activada
$sql = "SELECT * FROM tarjetas WHERE codigo = ? AND activado = 1";
$stmt = $conn->prepare($sql);
$stmt->bind_param("s", $codigo); // Vincula el parámetro de tipo string
$stmt->execute();
$result = $stmt->get_result();

if ($result->num_rows > 0) {
    // La tarjeta es válida, insertar en la tabla tarjetas_respuesta
    $aceptado = 1;  // La tarjeta es aceptada
    $row = $result->fetch_assoc();
    $id_tarjeta = $row['id'];
    
    $stmt_insert = $conn->prepare("INSERT INTO tarjetas_respuesta (codigo_tarjeta, aceptado) VALUES (?, ?)");
    $stmt_insert->bind_param("ii",$codigo, $aceptado);
    
    if ($stmt_insert->execute()) {
        echo "Valido";
    } else {
        echo "Error al registrar la respuesta";
    }
    
    $stmt_insert->close();
} else {
    // La tarjeta es inválida, insertar en la tabla tarjetas_respuesta con aceptado = 0
    $aceptado = 0;  // La tarjeta no es aceptada
    
    // No encontramos la tarjeta, insertamos con código y aceptado = 0
    $stmt_insert = $conn->prepare("INSERT INTO tarjetas_respuesta (codigo_tarjeta, aceptado) VALUES (?, ?)");
    $stmt_insert->bind_param("ii", $codigo, $aceptado); // Usar el código como id (si quieres manejarlo así)
    
    if ($stmt_insert->execute()) {
        echo "Invalido";
    } else {
        echo "Error al registrar la respuesta";
    }
    
    $stmt_insert->close();
}

$stmt->close();
$conn->close();
?>
