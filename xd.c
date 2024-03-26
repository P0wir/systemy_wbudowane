if (currentS7 != prevS7) {
            if (currentS7 == 1) {
                // Przesuń kolej w prawo
                portValue >>= 1;
                // Jeśli kolejka zmniejszy się poniżej 0000001, ustaw ją na 1100000
                if (portValue < 0b0000001) {
                    portValue = 0b1100000;
                }
            }
        }

