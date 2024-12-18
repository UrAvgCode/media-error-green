- wie kann man ein Bild zentrieren check
- wie kann man die Pixel aus dem Bild zu vielen Vectoren ändern
- Hintergrund vom Logo transparent machen
- wie schaffe ich es, dass alle Vectoren drumherum sich bewegen, nur die in der mitte nicht

- if (std::find(logoPositions.begin(), logoPositions.end(), currentPos) != logoPositions.end()) {
	continue;
}
else {
	float angle = ofNoise(x * 0.1, y * 0.1, zOffset) * TWO_PI; // Noise erzeugt Winkel
	flowField[y * cols + x] = ofVec2f(cos(angle), sin(angle));
}

