package com.example.smarthomeautomation.ui

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.bluetooth.BluetoothProfile
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.content.pm.PackageManager
import android.os.Build
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.animation.AnimatedVisibility
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalLayoutDirection
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.LayoutDirection
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.core.content.ContextCompat

// Vibrant Light Palette
val BackgroundColor = Color(0xFFF3F4F6) // Light Gray
val TextPrimary = Color(0xFF1F2937)     // Dark Gray
val TextSecondary = Color(0xFF6B7280)   // Medium Gray
val TextWhite = Color(0xFFFFFFFF)

val VibrantOrange = Color(0xFFFF9F43)
val VibrantPurple = Color(0xFF9B59B6)
val VibrantGreen = Color(0xFF2ECC71)
val VibrantCyan = Color(0xFF00D2D3)
val VibrantPink = Color(0xFFFF9FF3)
val VibrantRed = Color(0xFFFF6B6B)
val CardWhite = Color(0xFFFFFFFF)

data class BluetoothState(
    val isEnabled: Boolean = false,
    val connectedDeviceName: String? = null
)

@Preview
@Composable
fun HomeScreen() {
    // Dynamic hoisted states
    var isConnected by remember { mutableStateOf(false) }
    var selectedMode by remember { mutableStateOf("Manual") }
    var isMainLightOn by remember { mutableStateOf(false) }
    var isCeilingFanOn by remember { mutableStateOf(true) }
    var isAlarmActive by remember { mutableStateOf(true) }
    val bluetoothState by rememberBluetoothState()
    // New states for device selection
    var showDeviceDialog by remember { mutableStateOf(false) }
    var selectedDeviceName by remember { mutableStateOf<String?>(null) }
    val pairedDevices = remember { mutableStateListOf<String>() }

    val permissionLauncher = rememberLauncherForActivityResult(
        ActivityResultContracts.RequestMultiplePermissions()
    ) { _ ->
        // Handle permissions
    }

    LaunchedEffect(Unit) {
        val permissions = mutableListOf<String>()
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            permissions.add(Manifest.permission.BLUETOOTH_CONNECT)
            permissions.add(Manifest.permission.BLUETOOTH_SCAN)
        } else {
            permissions.add(Manifest.permission.BLUETOOTH)
            permissions.add(Manifest.permission.ACCESS_FINE_LOCATION)
        }
        permissionLauncher.launch(permissions.toTypedArray())
    }

    CompositionLocalProvider(LocalLayoutDirection provides LayoutDirection.Ltr) {
        Column(
            modifier = Modifier
                .fillMaxSize()
                .background(BackgroundColor)
                .padding(horizontal = 24.dp, vertical = 40.dp),
            verticalArrangement = Arrangement.spacedBy(32.dp)
        ) {
            HeaderSection(
                onToggleConnection = { isConnected = !isConnected },
                bluetoothState = bluetoothState,
                showDeviceDialog = showDeviceDialog,
                onShowDeviceDialogChange = { showDeviceDialog = it },
                selectedDeviceName = selectedDeviceName,
                onSelectedDeviceNameChange = { selectedDeviceName = it },
                pairedDevices = pairedDevices
            )
            SensorsDashboard()
            SwitchControlsSection(
                selectedMode = selectedMode,
                onModeSelected = { mode -> selectedMode = mode },
                isMainLightOn = isMainLightOn,
                onMainLightToggle = { isMainLightOn = it },
                isCeilingFanOn = isCeilingFanOn,
                onCeilingFanToggle = { isCeilingFanOn = it }
            )
            
            AnimatedVisibility(visible = isAlarmActive) {
                AlarmControlCard(onDismiss = { isAlarmActive = false })
            }
        }
    }
}

@Composable
fun HeaderSection(
    onToggleConnection: () -> Unit,
    bluetoothState: BluetoothState,
    showDeviceDialog: Boolean,
    onShowDeviceDialogChange: (Boolean) -> Unit,
    selectedDeviceName: String?,
    onSelectedDeviceNameChange: (String?) -> Unit,
    pairedDevices: MutableList<String>
) {
    Column(
        modifier = Modifier.fillMaxWidth(),
        verticalArrangement = Arrangement.spacedBy(20.dp)
    ) {
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.Top
        ) {
            Column {
                Text(
                    "Welcome Home",
                    color = TextPrimary,
                    fontSize = 32.sp,
                    fontWeight = FontWeight.Black
                )
                Text(
                    text = bluetoothState.connectedDeviceName ?: "Not connected",
                    color = TextSecondary,
                    fontSize = 16.sp,
                    fontWeight = FontWeight.Medium
                )
            }

            // Connection Status Pill
            val isDeviceConnected = bluetoothState.connectedDeviceName != null
            val pillColor = when {
                isDeviceConnected -> VibrantCyan
                bluetoothState.isEnabled -> VibrantGreen
                else -> VibrantRed
            }
            val pillText = if (isDeviceConnected) "Connected" else if (bluetoothState.isEnabled) "Ready" else "Offline"

            Surface(
                color = pillColor.copy(alpha = 0.1f),
                shape = RoundedCornerShape(12.dp),
                modifier = Modifier.clickable { onToggleConnection() }
            ) {
                Row(
                    modifier = Modifier.padding(horizontal = 12.dp, vertical = 6.dp),
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Box(
                        modifier = Modifier
                            .size(8.dp)
                            .clip(CircleShape)
                            .background(pillColor)
                    )
                    Spacer(modifier = Modifier.width(8.dp))
                    Text(pillText, color = pillColor, fontSize = 13.sp, fontWeight = FontWeight.Bold)
                }
            }
        }

        // Bluetooth Selection Card (Separate section under Title)
        Card(
            modifier = Modifier.fillMaxWidth(),
            shape = RoundedCornerShape(24.dp),
            colors = CardDefaults.cardColors(containerColor = CardWhite),
            elevation = CardDefaults.cardElevation(defaultElevation = 2.dp)
        ) {
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(20.dp),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Column(modifier = Modifier.weight(1f)) {
                    Text("Target ESP32 Device", color = TextSecondary, fontSize = 12.sp, fontWeight = FontWeight.Bold)
                    Spacer(modifier = Modifier.height(4.dp))
                    Text(
                        selectedDeviceName ?: "None selected",
                        color = TextPrimary,
                        fontSize = 18.sp,
                        fontWeight = FontWeight.ExtraBold
                    )
                }
                
                Button(
                    onClick = {
                        @Suppress("DEPRECATION")
                        val adapter = BluetoothAdapter.getDefaultAdapter()
                        val devices = try {
                            adapter?.bondedDevices?.map { it.name ?: "Unknown" } ?: emptyList()
                        } catch (e: SecurityException) {
                            emptyList()
                        }
                        pairedDevices.clear()
                        pairedDevices.addAll(devices)
                        onShowDeviceDialogChange(true)
                    },
                    colors = ButtonDefaults.buttonColors(containerColor = VibrantCyan),
                    shape = RoundedCornerShape(16.dp),
                    contentPadding = PaddingValues(horizontal = 20.dp, vertical = 12.dp)
                ) {
                    Text("SELECT", color = TextWhite, fontWeight = FontWeight.Black)
                }
            }
        }

        // Device selection dialog
        if (showDeviceDialog) {
            AlertDialog(
                onDismissRequest = { onShowDeviceDialogChange(false) },
                title = { Text("Available Devices", fontWeight = FontWeight.Black) },
                text = {
                    Column(modifier = Modifier.fillMaxWidth().heightIn(max = 300.dp)) {
                        if (pairedDevices.isEmpty()) {
                            Text("No paired devices found. Please pair in system settings first.", color = TextSecondary)
                        } else {
                            pairedDevices.forEach { deviceName ->
                                Surface(
                                    modifier = Modifier
                                        .fillMaxWidth()
                                        .clickable {
                                            onSelectedDeviceNameChange(deviceName)
                                            onShowDeviceDialogChange(false)
                                        },
                                    color = Color.Transparent
                                ) {
                                    Row(
                                        modifier = Modifier.padding(vertical = 12.dp, horizontal = 4.dp),
                                        verticalAlignment = Alignment.CenterVertically
                                    ) {
                                        Icon(Icons.Default.Info, contentDescription = null, tint = VibrantCyan, modifier = Modifier.size(20.dp))
                                        Spacer(modifier = Modifier.width(16.dp))
                                        Text(deviceName, color = TextPrimary, fontSize = 16.sp, fontWeight = FontWeight.Bold)
                                    }
                                }
                                HorizontalDivider(color = BackgroundColor.copy(alpha = 0.5f))
                            }
                        }
                    }
                },
                confirmButton = {},
                dismissButton = {
                    TextButton(onClick = { onShowDeviceDialogChange(false) }) {
                        Text("CANCEL", fontWeight = FontWeight.Bold)
                    }
                },
                shape = RoundedCornerShape(28.dp),
                containerColor = CardWhite
            )
        }
    }
}

@Composable
fun SensorsDashboard() {
    Column(verticalArrangement = Arrangement.spacedBy(16.dp)) {
        Text("Environment", color = TextPrimary, fontSize = 20.sp, fontWeight = FontWeight.Bold)
        
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.spacedBy(16.dp)
        ) {
            // Temperature Card - Vibrant Orange Solid
            Card(
                modifier = Modifier.weight(1f).height(140.dp),
                shape = RoundedCornerShape(24.dp),
                colors = CardDefaults.cardColors(containerColor = VibrantOrange)
            ) {
                Column(
                    modifier = Modifier.padding(20.dp).fillMaxSize(),
                    verticalArrangement = Arrangement.SpaceBetween
                ) {
                    Row(verticalAlignment = Alignment.CenterVertically) {
                        Box(
                            modifier = Modifier
                                .size(36.dp)
                                .clip(CircleShape)
                                .background(TextWhite.copy(alpha = 0.2f)),
                            contentAlignment = Alignment.Center
                        ) {
                            Icon(Icons.Default.Info, contentDescription = null, tint = TextWhite, modifier = Modifier.size(20.dp))
                        }
                        Spacer(modifier = Modifier.width(8.dp))
                        Text("Climate", color = TextWhite.copy(alpha = 0.9f), fontSize = 14.sp, fontWeight = FontWeight.SemiBold)
                    }
                    Text("28°C", color = TextWhite, fontSize = 36.sp, fontWeight = FontWeight.Black)
                }
            }
            
            // Security Summary Card - Vibrant Purple Solid
            Card(
                modifier = Modifier.weight(1f).height(140.dp),
                shape = RoundedCornerShape(24.dp),
                colors = CardDefaults.cardColors(containerColor = VibrantPurple)
            ) {
                Column(
                    modifier = Modifier.padding(20.dp).fillMaxSize(),
                    verticalArrangement = Arrangement.SpaceBetween
                ) {
                    Row(verticalAlignment = Alignment.CenterVertically) {
                        Box(
                            modifier = Modifier
                                .size(36.dp)
                                .clip(CircleShape)
                                .background(TextWhite.copy(alpha = 0.2f)),
                            contentAlignment = Alignment.Center
                        ) {
                            Icon(Icons.Default.Lock, contentDescription = null, tint = TextWhite, modifier = Modifier.size(20.dp))
                        }
                        Spacer(modifier = Modifier.width(8.dp))
                        Text("Security", color = TextWhite.copy(alpha = 0.9f), fontSize = 14.sp, fontWeight = FontWeight.SemiBold)
                    }
                    Column(verticalArrangement = Arrangement.spacedBy(6.dp)) {
                        SecurityStatusRow("Smoke", isActive = false)
                        SecurityStatusRow("Motion", isActive = false)
                        SecurityStatusRow("Touch", isActive = false)
                    }
                }
            }
        }
    }
}

@Composable
fun SecurityStatusRow(label: String, isActive: Boolean) {
    Row(verticalAlignment = Alignment.CenterVertically) {
        val color = if (isActive) VibrantRed else TextWhite
        Box(
            modifier = Modifier
                .size(8.dp)
                .clip(CircleShape)
                .background(if (isActive) VibrantRed else TextWhite.copy(alpha = 0.4f))
        )
        Spacer(modifier = Modifier.width(8.dp))
        Text(label, color = color, fontSize = 13.sp, fontWeight = FontWeight.Medium)
    }
}

@Composable
fun SwitchControlsSection(
    selectedMode: String,
    onModeSelected: (String) -> Unit,
    isMainLightOn: Boolean,
    onMainLightToggle: (Boolean) -> Unit,
    isCeilingFanOn: Boolean,
    onCeilingFanToggle: (Boolean) -> Unit
) {
    Column(verticalArrangement = Arrangement.spacedBy(16.dp)) {
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Text("Devices", color = TextPrimary, fontSize = 20.sp, fontWeight = FontWeight.Bold)
            
            // Mode Segmented Button
            Row(
                modifier = Modifier
                    .clip(RoundedCornerShape(16.dp))
                    .background(CardWhite)
                    .padding(4.dp)
            ) {
                listOf("Manual", "Auto", "Off").forEach { mode ->
                    ModeChip(
                        text = mode,
                        isSelected = selectedMode == mode,
                        onClick = { onModeSelected(mode) }
                    )
                }
            }
        }
        
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.spacedBy(16.dp)
        ) {
            // Main Light Card
            DeviceCard(
                modifier = Modifier.weight(1f),
                deviceName = "Main Light",
                icon = Icons.Default.Build,
                isOn = isMainLightOn,
                onToggle = onMainLightToggle,
                accentColor = VibrantPink
            )
            // Ceiling Fan Card
            DeviceCard(
                modifier = Modifier.weight(1f),
                deviceName = "Ceiling Fan",
                icon = Icons.Default.Build,
                isOn = isCeilingFanOn,
                onToggle = onCeilingFanToggle,
                accentColor = VibrantCyan
            )
        }
    }
}

@Composable
fun ModeChip(text: String, isSelected: Boolean, onClick: () -> Unit) {
    Box(
        modifier = Modifier
            .clip(RoundedCornerShape(12.dp))
            .background(if (isSelected) VibrantGreen else Color.Transparent)
            .clickable { onClick() }
            .padding(horizontal = 14.dp, vertical = 6.dp)
    ) {
        Text(
            text = text,
            color = if (isSelected) TextWhite else TextSecondary,
            fontSize = 13.sp,
            fontWeight = FontWeight.Bold
        )
    }
}

@Composable
fun DeviceCard(
    modifier: Modifier, 
    deviceName: String, 
    icon: ImageVector, 
    isOn: Boolean, 
    onToggle: (Boolean) -> Unit,
    accentColor: Color
) {
    val activeColor = if (isOn) accentColor else TextSecondary
    val bgColor = if (isOn) accentColor.copy(alpha = 0.1f) else CardWhite
    
    Card(
        modifier = modifier.height(150.dp).clickable { onToggle(!isOn) },
        shape = RoundedCornerShape(24.dp),
        colors = CardDefaults.cardColors(containerColor = bgColor)
    ) {
        Column(
            modifier = Modifier.padding(20.dp).fillMaxSize(),
            verticalArrangement = Arrangement.SpaceBetween
        ) {
            Row(
                modifier = Modifier.fillMaxWidth(), 
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.Top
            ) {
                Box(
                    modifier = Modifier
                        .size(48.dp)
                        .clip(CircleShape)
                        .background(if (isOn) accentColor else BackgroundColor),
                    contentAlignment = Alignment.Center
                ) {
                    Icon(icon, contentDescription = null, tint = if (isOn) TextWhite else TextSecondary, modifier = Modifier.size(24.dp))
                }
                
                Switch(
                    checked = isOn,
                    onCheckedChange = onToggle,
                    colors = SwitchDefaults.colors(
                        checkedThumbColor = TextWhite,
                        checkedTrackColor = accentColor,
                        uncheckedThumbColor = BackgroundColor,
                        uncheckedTrackColor = TextSecondary.copy(alpha = 0.3f),
                        uncheckedBorderColor = Color.Transparent
                    )
                )
            }
            
            Column {
                Text(deviceName, color = TextPrimary, fontSize = 16.sp, fontWeight = FontWeight.Bold)
                Spacer(modifier = Modifier.height(4.dp))
                Text(if (isOn) "ON" else "OFF", color = activeColor, fontSize = 13.sp, fontWeight = FontWeight.Black)
            }
        }
    }
}

@Composable
fun AlarmControlCard(onDismiss: () -> Unit) {
    Card(
        modifier = Modifier.fillMaxWidth().height(90.dp),
        shape = RoundedCornerShape(24.dp),
        colors = CardDefaults.cardColors(containerColor = VibrantRed)
    ) {
        Row(
            modifier = Modifier.padding(16.dp).fillMaxSize(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Row(verticalAlignment = Alignment.CenterVertically) {
                Box(
                    modifier = Modifier.size(54.dp).clip(CircleShape).background(TextWhite.copy(alpha = 0.2f)),
                    contentAlignment = Alignment.Center
                ) {
                    Icon(Icons.Default.Warning, contentDescription = null, tint = TextWhite, modifier = Modifier.size(28.dp))
                }
                Spacer(modifier = Modifier.width(16.dp))
                Column {
                    Text("Emergency Alarm", color = TextWhite, fontSize = 18.sp, fontWeight = FontWeight.Bold)
                    Text("Touch sensor active", color = TextWhite.copy(alpha = 0.8f), fontSize = 13.sp)
                }
            }
            
            Button(
                onClick = onDismiss,
                colors = ButtonDefaults.buttonColors(containerColor = TextWhite),
                shape = RoundedCornerShape(16.dp)
            ) {
                Text("DISMISS", color = VibrantRed, fontWeight = FontWeight.Black, fontSize = 12.sp)
            }
        }
    }
}

@Composable
fun rememberBluetoothState(): State<BluetoothState> {
    val context = LocalContext.current
    val bluetoothState = remember { mutableStateOf(BluetoothState()) }

    fun checkConnectedDevice(adapter: BluetoothAdapter?) {
        if (adapter == null || !adapter.isEnabled) {
            bluetoothState.value = bluetoothState.value.copy(connectedDeviceName = null)
            return
        }

        val hasPermission = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            ContextCompat.checkSelfPermission(context, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED
        } else {
            true
        }

        if (hasPermission) {
            // Check common profiles first as a proxy
            val proxyCallback = object : BluetoothProfile.ServiceListener {
                override fun onServiceConnected(profile: Int, proxy: BluetoothProfile?) {
                    val devices = proxy?.connectedDevices
                    if (!devices.isNullOrEmpty()) {
                        bluetoothState.value = bluetoothState.value.copy(connectedDeviceName = try { devices[0].name } catch(e: SecurityException) { "Unknown" })
                    }
                    @Suppress("DEPRECATION")
                    BluetoothAdapter.getDefaultAdapter()?.closeProfileProxy(profile, proxy)
                }
                override fun onServiceDisconnected(profile: Int) {}
            }
            
            adapter.getProfileProxy(context, proxyCallback, BluetoothProfile.A2DP)
            adapter.getProfileProxy(context, proxyCallback, BluetoothProfile.HEADSET)
        }
    }

    DisposableEffect(context) {
        @Suppress("DEPRECATION")
        val adapter = BluetoothAdapter.getDefaultAdapter()
        bluetoothState.value = BluetoothState(isEnabled = adapter?.isEnabled == true)
        
        checkConnectedDevice(adapter)

        val receiver = object : BroadcastReceiver() {
            override fun onReceive(context: Context?, intent: Intent?) {
                when (intent?.action) {
                    BluetoothAdapter.ACTION_STATE_CHANGED -> {
                        val state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR)
                        val isEnabled = (state == BluetoothAdapter.STATE_ON)
                        bluetoothState.value = bluetoothState.value.copy(isEnabled = isEnabled)
                        if (isEnabled) checkConnectedDevice(adapter)
                    }
                    BluetoothDevice.ACTION_ACL_CONNECTED -> {
                        val device = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                            intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE, BluetoothDevice::class.java)
                        } else {
                            @Suppress("DEPRECATION")
                            intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE)
                        }
                        
                        val hasPermission = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                            ContextCompat.checkSelfPermission(context!!, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED
                        } else {
                            true
                        }
                        
                        val name = if (hasPermission) {
                            try { device?.name } catch(e: SecurityException) { null }
                        } else {
                            null
                        }
                        bluetoothState.value = bluetoothState.value.copy(connectedDeviceName = name ?: "Connected Device")
                    }
                    BluetoothDevice.ACTION_ACL_DISCONNECTED -> {
                        bluetoothState.value = bluetoothState.value.copy(connectedDeviceName = null)
                    }
                }
            }
        }
        
        val filter = IntentFilter().apply {
            addAction(BluetoothAdapter.ACTION_STATE_CHANGED)
            addAction(BluetoothDevice.ACTION_ACL_CONNECTED)
            addAction(BluetoothDevice.ACTION_ACL_DISCONNECTED)
        }
        context.registerReceiver(receiver, filter)

        onDispose {
            try {
                context.unregisterReceiver(receiver)
            } catch (e: Exception) {}
        }
    }
    return bluetoothState
}
