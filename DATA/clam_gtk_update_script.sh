#!/bin/bash
systemctl stop clamav-freshclam
freshclam
systemctl start clamav-freshclam