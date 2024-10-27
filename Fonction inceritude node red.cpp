Incertitudes

// Récupération de la masse mesurée (reading) depuis le message MQTT
var reading = msg.payload;  // La valeur de la masse reçue

// Définition des incertitudes associées pour chaque plage
var incertitude_100g = 0.18;   // Incertitude pour la plage autour de 100g
var incertitude_200g = 0.17;  // Incertitude pour la plage autour de 200g
var incertitude_300g = 0.16;  // Incertitude pour la plage autour de 300g
var incertitude_400g = 0.18;  // Incertitude pour la plage autour de 400g
var incertitude_500g = 0.17;  // Incertitude pour la plage autour de 500g
var incertitude_600g = 0.22;  // Incertitude pour la plage autour de 600g
var incertitude_700g = 0.33;  // Incertitude pour la plage autour de 700g
var incertitude_800g = 0.42;  // Incertitude pour la plage autour de 800g

var result;
// Vérification des plages de masse
if (reading >= 95 && reading <= 150) {
    // Si la masse est entre 95g et 150g, utiliser l'incertitude pour 100g
    result = reading + "g ± " + incertitude_100g + "g";
} else if (reading > 150 && reading <= 250) {
    // Si la masse est entre 150g et 250g, utiliser l'incertitude pour 200g
    result = reading + "g ± " + incertitude_200g + "g";
} else if (reading > 250 && reading <= 350) {
    // Si la masse est entre 250g et 350g, utiliser l'incertitude pour 300g
    result = reading + "g ± " + incertitude_300g + "g";
} else if (reading > 350 && reading <= 450) {
    // Si la masse est entre 350g et 450g, utiliser l'incertitude pour 400g
    result = reading + "g ± " + incertitude_400g + "g";
} else if (reading > 450 && reading <= 550) {
    // Si la masse est entre 450g et 550g, utiliser l'incertitude pour 500g
    result = reading + "g ± " + incertitude_500g + "g";
} else if (reading > 550 && reading <= 650) {
    // Si la masse est entre 550g et 650g, utiliser l'incertitude pour 600g
    result = reading + "g ± " + incertitude_600g + "g";
} else if (reading > 650 && reading <= 750) {
    // Si la masse est entre 650g et 750g, utiliser l'incertitude pour 700g
    result = reading + "g ± " + incertitude_700g + "g";
} else if (reading > 750 && reading <= 850) {
    // Si la masse est entre 750g et 850g, utiliser l'incertitude pour 800g
    result = reading + "g ± " + incertitude_800g + "g";
} else {
    // Si la masse est en dehors des plages spécifiées, afficher la valeur brute
    result = "Masse mesurée : " + reading + "g (hors plage d'incertitude)";
}

// Renvoi du résultat pour l'afficher dans Node-RED
msg.payload = result;
return msg;
